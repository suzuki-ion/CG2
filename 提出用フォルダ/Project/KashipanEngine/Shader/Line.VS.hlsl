#include "Line.hlsli"
#include "TransformationMatrix.hlsli"

struct LineOption {
	int type;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<LineOption> gLineOption : register(b1);

struct VertexShaderInput {
	float4 pos : POSITION0;
	float4 color : COLOR0;
	float width : WIDTH0;
	float height : HEIGHT0;
	float depth : DEPTH0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	if (gLineOption.type == 0) {
		output.pos = mul(input.pos, gTransformationMatrix.WVP);
	} else {
		output.pos = input.pos;
	}
	output.color = input.color;
	output.width = input.width;
	output.height = input.height;
	output.depth = input.depth;
	output.wvp = gTransformationMatrix.WVP;
	return output;
}