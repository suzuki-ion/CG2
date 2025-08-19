struct VertexInput {
	float4 position : POSITION0;
};

struct VertexOutput {
	float4 position : SV_POSITION;
};

VertexOutput main(VertexInput input) {
	VertexOutput output;
	output.position = input.position;
	return output;
}