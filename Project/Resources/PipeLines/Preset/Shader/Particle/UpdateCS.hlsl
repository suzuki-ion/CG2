#include "TransformationMatrix.hlsli"

struct GpuTransform {
	float3 scale;
	float _pad0;
	float3 rotate;
	float _pad1;
	float3 translate;
	float _pad2;
};

struct GpuMaterial {
	float4 color;
	int lightingType;
	int _pad0[3];
	float4x4 uvTransform;
	float4 diffuseColor;
	float4 specularColor;
	float4 emissiveColor;
};

struct GpuParticle {
	uint isActive;
	uint isAlive;
	int textureIndex;
	int _pad0;
	GpuTransform transform;
	GpuMaterial material;
	float3 velocity;
	float _padV;
};

RWStructuredBuffer<TransformationMatrix> gMatricesOut : register(u0);
RWStructuredBuffer<GpuParticle> gParticles : register(u1);

cbuffer ParticleUpdateCB : register(b0) {
	float deltaTime;
	float3 _pad0;
	float4 spawnPos;
	uint maxCount;
	float _pad1;
	uint instanceCount;
	float noiseTime;
	float4x4 viewProjection;
	
	float noiseScale;
	float noiseAmplitudeBase;
	float timeSpeedX;
	float timeSpeedY;
	float timeSpeedZ;
	float curlStrength;
	float damping;
	float eps;
	float3 boxHalfExtent;
}

float HashFloat3(float3 p) {
	return frac(sin(dot(p, float3(127.1, 311.7, 74.7))) * 43758.5453);
}

float Noise3(float3 p) {
	float3 i = floor(p);
	float3 f = frac(p);
	float3 u = f * f * (3.0 - 2.0 * f);

	float n000 = HashFloat3(i + float3(0.0, 0.0, 0.0));
	float n100 = HashFloat3(i + float3(1.0, 0.0, 0.0));
	float n010 = HashFloat3(i + float3(0.0, 1.0, 0.0));
	float n110 = HashFloat3(i + float3(1.0, 1.0, 0.0));
	float n001 = HashFloat3(i + float3(0.0, 0.0, 1.0));
	float n101 = HashFloat3(i + float3(1.0, 0.0, 1.0));
	float n011 = HashFloat3(i + float3(0.0, 1.0, 1.0));
	float n111 = HashFloat3(i + float3(1.0, 1.0, 1.0));

	float nx00 = lerp(n000, n100, u.x);
	float nx10 = lerp(n010, n110, u.x);
	float nx01 = lerp(n001, n101, u.x);
	float nx11 = lerp(n011, n111, u.x);

	float nxy0 = lerp(nx00, nx10, u.y);
	float nxy1 = lerp(nx01, nx11, u.y);

	return lerp(nxy0, nxy1, u.z);
}

float Fbm(float3 p) {
	float value = 0.0;
	float amplitude = 0.5;
	for (int i = 0; i < 4; ++i) {
		value += amplitude * Noise3(p);
		p *= 2.0;
		amplitude *= 0.5;
	}
	return value;
}

float3 CurlNoise(float3 p) {
	float epsLocal = max(eps, 1e-5);
	float3 dx = float3(epsLocal, 0.0, 0.0);
	float3 dy = float3(0.0, epsLocal, 0.0);
	float3 dz = float3(0.0, 0.0, epsLocal);
	
	const float3 o1 = float3(12.9898, 78.233, 45.164);
	const float3 o2 = float3(31.4159, 61.4159, 14.159);
	
	float fx1 = Fbm(p + dx);
	float fx0 = Fbm(p - dx);
	float fy1 = Fbm(p + dy);
	float fy0 = Fbm(p - dy);
	float fz1 = Fbm(p + dz);
	float fz0 = Fbm(p - dz);

	float gx1 = Fbm(p + o1 + dx);
	float gx0 = Fbm(p + o1 - dx);
	float gy1 = Fbm(p + o1 + dy);
	float gy0 = Fbm(p + o1 - dy);
	float gz1 = Fbm(p + o1 + dz);
	float gz0 = Fbm(p + o1 - dz);

	float Hx1 = Fbm(p + o2 + dx);
	float Hx0 = Fbm(p + o2 - dx);
	float Hy1 = Fbm(p + o2 + dy);
	float Hy0 = Fbm(p + o2 - dy);
	float Hz1 = Fbm(p + o2 + dz);
	float Hz0 = Fbm(p + o2 - dz);

	float dfdx = (fx1 - fx0) / (2.0 * epsLocal);
	float dfdy = (fy1 - fy0) / (2.0 * epsLocal);
	float dfdz = (fz1 - fz0) / (2.0 * epsLocal);

	float dgdx = (gx1 - gx0) / (2.0 * epsLocal);
	float dgdy = (gy1 - gy0) / (2.0 * epsLocal);
	float dgdz = (gz1 - gz0) / (2.0 * epsLocal);

	float dhdx = (Hx1 - Hx0) / (2.0 * epsLocal);
	float dhdy = (Hy1 - Hy0) / (2.0 * epsLocal);
	float dhdz = (Hz1 - Hz0) / (2.0 * epsLocal);
	
	return float3(dhdy - dgdz, dfdz - dhdx, dgdx - dfdy);
}

float4x4 MakeTranslate(float3 t) {
	return float4x4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        t.x, t.y, t.z, 1.0);
}

float4x4 MakeScale(float3 s) {
	return float4x4(
		s.x, 0.0, 0.0, 0.0,
		0.0, s.y, 0.0, 0.0,
		0.0, 0.0, s.z, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

float4x4 MakeRotateX(float a) {
	float c = cos(a);
	float s = sin(a);
	return float4x4(
		1.0, 0.0, 0.0, 0.0,
		0.0, c, -s, 0.0,
		0.0, s, c, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

float4x4 MakeRotateY(float a) {
	float c = cos(a);
	float s = sin(a);
	return float4x4(
		 c, 0.0, s, 0.0,
		0.0, 1.0, 0.0, 0.0,
		-s, 0.0, c, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

float4x4 MakeRotateZ(float a) {
	float c = cos(a);
	float s = sin(a);
	return float4x4(
		 c, -s, 0.0, 0.0,
		 s, c, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

float4x4 MakeRotate(float3 r) {
	float4x4 rz = MakeRotateZ(r.z);
	float4x4 ry = MakeRotateY(r.y);
	float4x4 rx = MakeRotateX(r.x);
	return mul(mul(rz, ry), rx);
}

[numthreads(1024, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID) {
	if (dtid.x >= instanceCount || gParticles[dtid.x].isActive == 0 || gParticles[dtid.x].isAlive == 0) {
		return;
	}

	float3 pos = gParticles[dtid.x].transform.translate;
	float3 rot = gParticles[dtid.x].transform.rotate;
	float3 scl = gParticles[dtid.x].transform.scale;
	float3 vel = gParticles[dtid.x].velocity;
	
	{
		float idf = float(dtid.x);
		int texIdx = gParticles[dtid.x].textureIndex;
		float s0 = HashFloat3(float3(idf, float(texIdx), 12.9898));
		float s1 = HashFloat3(float3(idf, float(texIdx), 78.233));
		float s2 = HashFloat3(float3(idf, float(texIdx), 45.164));
		float3 seed = float3(s0, s1, s2);

		float3 samplePos = pos * noiseScale + seed * 10.0;
		float3 curl = CurlNoise(samplePos + float3(noiseTime, noiseTime * 0.7, noiseTime * 1.3));
		
		vel = vel * damping + normalize(curl) * curlStrength * deltaTime;
		
		if (all(vel == float3(0.0,0.0,0.0))) {
			vel = (seed - 0.5f) * 0.01f;
		}

		{
			float3 p = pos * noiseScale + seed * 10.0;
			float nx = Fbm(p + float3(noiseTime * timeSpeedX + seed.x * 10.0, 0.0, 0.0));
			float ny = Fbm(p + float3(0.0, noiseTime * timeSpeedY + seed.y * 10.0, 0.0));
			float nz = Fbm(p + float3(0.0, 0.0, noiseTime * timeSpeedZ + seed.z * 10.0));
			float3 nvec = float3(nx, ny, nz) - 0.5f;
			float noiseAmplitude = noiseAmplitudeBase * (0.5f + seed.y);
			pos += nvec * noiseAmplitude * deltaTime * 0.1f;
		}
		{
			float3 r = rot * noiseScale + seed * 5.0;
			float nx = Fbm(r + float3(noiseTime * timeSpeedX + seed.x * 5.0, 0.0, 0.0));
			float ny = Fbm(r + float3(0.0, noiseTime * timeSpeedY + seed.y * 5.0, 0.0));
			float nz = Fbm(r + float3(0.0, 0.0, noiseTime * timeSpeedZ + seed.z * 5.0));
			float3 nvec = float3(nx, ny, nz) - 0.5f;
			float rotAmp = 1.0f + seed.x;
			rot += nvec * rotAmp * deltaTime;
		}
	}
	
	pos += vel * deltaTime;

	if (any(abs(pos) > boxHalfExtent)) {
		pos = spawnPos.xyz;
		vel = float3(0.0, 0.0, 0.0);
	}

	gParticles[dtid.x].transform.translate = pos;
	gParticles[dtid.x].transform.rotate = rot;
	gParticles[dtid.x].transform.scale = scl;
	gParticles[dtid.x].velocity = vel;
	
	float4x4 scaleM = MakeScale(scl);
	float4x4 rotM = MakeRotate(rot);
	float4x4 transM = MakeTranslate(pos);
	float4x4 world = mul(mul(scaleM, rotM), transM);

	gMatricesOut[dtid.x].World = world;
	gMatricesOut[dtid.x].WVP = mul(world, viewProjection);
	gMatricesOut[dtid.x].ViewportInverse = (float4x4) 0;
}