#include "Particle.hlsli"
#include "TransformationMatrix.hlsli"

StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t0);

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID) {
	VertexShaderOutput output;
	output.position = mul(input.position, gTransformationMatrices[instanceID].WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrices[instanceID].World));
	return output;
}