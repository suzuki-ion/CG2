float HalfLambert(float3 normal, float3 lightDirection) {
	float NdotL = dot(normal, lightDirection);
	return pow(max(NdotL, 0.0f) * 0.5f + 0.5f, 2.0f);
}

float Phong(float3 normal, float3 lightDirection, float3 viewDirection, float shininess) {
	float NdotL = dot(normal, lightDirection);
	float3 reflection = normalize(-lightDirection + 2.0f * normal * NdotL);
	float RdotV = dot(reflection, viewDirection);
	return pow(max(RdotV, 0.0f), shininess);
}

float Schlick(float3 normal, float3 viewDirection, float reflectance) {
	float VdotN = dot(viewDirection, normal);
	return reflectance + (1.0f - reflectance) * pow(1.0f - VdotN, 5.0f);
}