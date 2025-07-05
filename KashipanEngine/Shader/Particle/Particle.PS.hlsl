#include "Particle.hlsli"
#include "../Lighting.hlsli"

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

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	
    if (gMaterial.enableLighting != 0) {
        float NdotL = dot(input.normal, gDirectionalLight.direction);
        float phong = Phong(input.normal, gDirectionalLight.direction, normalize(-input.position.xyz), gDirectionalLight.intensity);
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL + (phong) * gDirectionalLight.color.a;
        output.color.a = gMaterial.color.a * textureColor.a;

    }
    else {
        output.color = gMaterial.color * textureColor;
    }
	
    return output;
}