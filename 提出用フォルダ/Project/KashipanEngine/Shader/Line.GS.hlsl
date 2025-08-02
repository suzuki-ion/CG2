#include "Line.hlsli"

[maxvertexcount(6)]
void main(line VertexShaderOutput input[2], inout TriangleStream<GeometryShaderOutput> output) {
	for (int i = 0; i < 2; i++) {
		float offsetWidth  = input[i].width / 2.0f;
		float offsetHeight = input[i].height / 2.0f;
		float offsetDepth  = input[i].depth / 2.0f;
		
        {
			GeometryShaderOutput element;
			element.pos = mul(input[i].pos + float4(offsetWidth, offsetHeight, offsetDepth, 0.0f), input[i].wvp);
			element.color	= input[i].color;
			output.Append(element);
		}
        {
			GeometryShaderOutput element;
			element.pos = mul(input[i].pos + float4(-offsetWidth, -offsetHeight, -offsetDepth, 0.0f), input[i].wvp);
			element.color	= input[i].color;
			output.Append(element);
		}
        {
			GeometryShaderOutput element;
			element.pos = mul(input[(i + 1) % 2].pos + float4(
				offsetWidth  * sign(float(i - 1)),
				offsetHeight * sign(float(i - 1)),
				offsetDepth  * sign(float(i - 1)),
				0.0f), input[i].wvp);
			element.color	= input[(i + 1) % 2].color;
			output.Append(element);
		}
		output.RestartStrip();
	}
}