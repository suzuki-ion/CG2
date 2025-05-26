#include "Object3d.hlsli"

cbuffer Light : register(b0) {
	float4x4 viewProjectionMatrix;
};
cbuffer Position : register(b1) {
	TransformationMatrix transformationMatrix;
};

float4 main(float4 position : POSITION0) : SV_POSITION {
	float4 output;
	output = mul(position, transformationMatrix.World);
	output = mul(output, viewProjectionMatrix);
	return output;
}