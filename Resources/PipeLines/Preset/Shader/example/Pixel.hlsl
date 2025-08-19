struct Test {
	float4 color;
	float4 position;
};

ConstantBuffer<Test> cb : register(b0);

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}