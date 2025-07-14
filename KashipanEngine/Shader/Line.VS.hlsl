#include "Line.hlsli"
#include "TransformationMatrix.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput {
	float4 pos : POSITION0;
	float4 color : COLOR0;
	float width : WIDTH0;
	float height : HEIGHT0;
	float depth : DEPTH0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.pos = input.pos;
	output.color = input.color;
	output.width = input.width;
	output.height = input.height;
	output.depth = input.depth;
	output.wvp = gTransformationMatrix.WVP;
	return output;
}