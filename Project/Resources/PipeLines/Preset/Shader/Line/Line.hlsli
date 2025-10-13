struct VertexShaderOutput {
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float width : WIDTH;
	float height : HEIGHT;
	float depth : DEPTH;
	float4x4 wvp : WORLDVIEWPROJECTION;
};

struct GeometryShaderOutput {
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};