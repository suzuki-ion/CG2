float4 RGBAtoHSVA(float4 rgba) {
	float4 hsva;
	float maxValue = max(max(rgba.r, rgba.g), rgba.b);
	float minValue = min(min(rgba.r, rgba.g), rgba.b);
	float delta = maxValue - minValue;
	hsva.a = rgba.a;
	
	// Value
	hsva.b = maxValue;
	
	if (maxValue == 0.0f) {
		hsva.r = 0.0f; // Hue
		hsva.g = 0.0f; // Saturation
		return hsva;
	}
	
	hsva.g = delta / maxValue; // Saturation
	
	if (delta == 0.0f) {
		hsva.r = 0.0f; // Hue
	}
	else {
		if (maxValue == rgba.r) {
			hsva.r = (rgba.g - rgba.b) / delta;
		}
		else if (maxValue == rgba.g) {
			hsva.r = (rgba.b - rgba.r) / delta + 2.0f;
		}
		else {
			hsva.r = (rgba.r - rgba.g) / delta + 4.0f;
		}
		
		hsva.r /= 6.0f; // Convert to degrees
		if (hsva.r < 0.0f) {
			hsva.r += 1.0f; // Ensure hue is positive
		}
	}
	
	return hsva;
}

float4 HSVAtoRGBA(float4 hsva) {
	if (hsva.g == 0.0f) {
		return float4(hsva.b, hsva.b, hsva.b, hsva.a); // Grayscale
	}
	
	hsva.r *= 6.0f; // Convert hue to degrees
	int i = int(hsva.r);
	float f = hsva.r - float(i);
	float p = hsva.b * (1.0f - hsva.g);
	float q = hsva.b * (1.0f - hsva.g * f);
	float t = hsva.b * (1.0f - hsva.g * (1.0f - f));
	
	float4 rgba;
	switch (i % 6) {
		case 0:
			rgba = float4(hsva.b, t, p, hsva.a);
			break;
		case 1:
			rgba = float4(q, hsva.b, p, hsva.a);
			break;
		case 2:
			rgba = float4(p, hsva.b, t, hsva.a);
			break;
		case 3:
			rgba = float4(p, q, hsva.b, hsva.a);
			break;
		case 4:
			rgba = float4(t, p, hsva.b, hsva.a);
			break;
		case 5:
			rgba = float4(hsva.b, p, q, hsva.a);
			break;
	}
	
	return rgba;
}

float4 ColorSaturation(float4 color, float saturation) {
	if (color.r == color.g ||
		color.g == color.b) {
		return color; // No change needed
	}
	float4 hsva = RGBAtoHSVA(color);
	hsva.g = saturate(saturation);
	return HSVAtoRGBA(hsva);
}