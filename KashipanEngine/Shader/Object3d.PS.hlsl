#include "Object3d.hlsli"

struct Material {
	float4 color;
	int enableLighting;
	float4x4 uvTransform;
	float4 diffuseColor;
	float4 specularColor;
	float4 emissiveColor;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

float4 RGBAtoHSVA(float4 rgba) {
	float4 hsva;
	float maxValue = max(max(rgba.r, rgba.g), rgba.b);
	float minValue = min(min(rgba.r, rgba.g), rgba.b);
	float delta = maxValue - minValue;
	hsva.a = rgba.a;
	
	// Value
	hsva.b = maxValue;
	
	if (maxValue == 0.0f) {
		hsva.r = 0.0f; // Hue
		hsva.g = 0.0f; // Saturation
		return hsva;
	}
	
	hsva.g = delta / maxValue; // Saturation
	
	if (delta == 0.0f) {
		hsva.r = 0.0f; // Hue
	} else {
		if (maxValue == rgba.r) {
			hsva.r = (rgba.g - rgba.b) / delta;
		}
		else if (maxValue == rgba.g) {
			hsva.r = (rgba.b - rgba.r) / delta + 2.0f;
		}
		else {
			hsva.r = (rgba.r - rgba.g) / delta + 4.0f;
		}
		
		hsva.r /= 6.0f; // Convert to degrees
		if (hsva.r < 0.0f) {
			hsva.r += 1.0f; // Ensure hue is positive
		}
	}
	
	return hsva;
}

float4 HSVAtoRGBA(float4 hsva) {
	if (hsva.g == 0.0f) {
		return float4(hsva.b, hsva.b, hsva.b, hsva.a); // Grayscale
	}
	
	hsva.r *= 6.0f; // Convert hue to degrees
	int i = int(hsva.r);
	float f = hsva.r - float(i);
	float p = hsva.b * (1.0f - hsva.g);
	float q = hsva.b * (1.0f - hsva.g * f);
	float t = hsva.b * (1.0f - hsva.g * (1.0f - f));
	
	float4 rgba;
	switch (i % 6) {
		case 0:
			rgba = float4(hsva.b, t, p, hsva.a);
			break;
		case 1:
			rgba = float4(q, hsva.b, p, hsva.a);
			break;
		case 2:
			rgba = float4(p, hsva.b, t, hsva.a);
			break;
		case 3:
			rgba = float4(p, q, hsva.b, hsva.a);
			break;
		case 4:
			rgba = float4(t, p, hsva.b, hsva.a);
			break;
		case 5:
			rgba = float4(hsva.b, p, q, hsva.a);
			break;
	}
	
	return rgba;
}

float4 ColorSaturation(float4 color, float saturation) {
	if (color.r == color.g ||
		color.g == color.b) {
		return color; // No change needed
	}
	float4 hsva = RGBAtoHSVA(color);
	hsva.g = saturate(saturation);
	return HSVAtoRGBA(hsva);
}

float HalfLambert(float3 normal, float3 lightDirection) {
	float NdotL = dot(normal, lightDirection);
	return pow(max(NdotL, 0.0f) * 0.5f + 0.5f, 2.0f);
}

float Phong(float3 normal, float3 lightDirection, float3 viewDirection, float shininess) {
	float NdotL = dot(normal, lightDirection);
	float3 reflection = normalize(-lightDirection + 2.0f * normal * NdotL);
	float RdotV = dot(reflection, viewDirection);
	return pow(max(RdotV, 0.0f), shininess);
}

float Schlick(float3 normal, float3 viewDirection, float reflectance) {
	float VdotN = dot(viewDirection, normal);
	return reflectance + (1.0f - reflectance) * pow(1.0f - VdotN, 5.0f);
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	
	if (gMaterial.enableLighting != 0) {
		float NdotL = dot(input.normal, gDirectionalLight.direction);
		float phong = Phong(input.normal, gDirectionalLight.direction, normalize(-input.position.xyz), gDirectionalLight.intensity);
		output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL + (phong) * gDirectionalLight.color.a;
		output.color.a = gMaterial.color.a * textureColor.a;

	} else {
		output.color = gMaterial.color * textureColor;
	}
	
	return output;
}