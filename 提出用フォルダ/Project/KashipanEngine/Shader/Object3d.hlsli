struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float4 positionShadow : POSITION_SM;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct DirectionalLight {
	float4 color;
	float3 direction;
	float intensity;
	float4x4 viewProjectionMatrix;
};