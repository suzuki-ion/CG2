#include <math.h>
#include "Easings.h"
#define M_PI (4.0f * atanf(1.0f))

namespace KashipanEngine {

const char *Ease::easeName_[EASINGS] = {
	"EASE_NONE",
	"EASE_IN_SINE",
	"EASE_OUT_SINE",
	"EASE_IN_OUT_SINE",
	"EASE_OUT_IN_SINE",
	"EASE_IN_QUAD",
	"EASE_OUT_QUAD",
	"EASE_IN_OUT_QUAD",
	"EASE_OUT_IN_QUAD",
	"EASE_IN_CUBIC",
	"EASE_OUT_CUBIC",
	"EASE_IN_OUT_CUBIC",
	"EASE_OUT_IN_CUBIC",
	"EASE_IN_QUART",
	"EASE_OUT_QUART",
	"EASE_IN_OUT_QUART",
	"EASE_OUT_IN_QUART",
	"EASE_IN_QUINT",
	"EASE_OUT_QUINT",
	"EASE_IN_OUT_QUINT",
	"EASE_OUT_IN_QUINT",
	"EASE_IN_EXPO",
	"EASE_OUT_EXPO",
	"EASE_IN_OUT_EXPO",
	"EASE_OUT_IN_EXPO",
	"EASE_IN_CIRC",
	"EASE_OUT_CIRC",
	"EASE_IN_OUT_CIRC",
	"EASE_OUT_IN_CIRC",
	"EASE_IN_BACK",
	"EASE_OUT_BACK",
	"EASE_IN_OUT_BACK",
	"EASE_OUT_IN_BACK",
	"EASE_IN_ELASTIC",
	"EASE_OUT_ELASTIC",
	"EASE_IN_OUT_ELASTIC",
	"EASE_OUT_IN_ELASTIC",
	"EASE_IN_BOUNCE",
	"EASE_OUT_BOUNCE",
	"EASE_IN_OUT_BOUNCE",
	"EASE_OUT_IN_BOUNCE",
	"EASE_LINEAR",
};

float Ease::Auto(int count, int countMax, float x1, float x2, int easeType, float scale) {
	float t = Count2Time(count, countMax);
	static float easing;
	t = powf(t, scale);
	switch (easeType) {
		case EASE_NONE:
            easing = None(t, x1, x2);
			break;

		case EASE_IN_SINE:
			easing = InSine(t, x1, x2);
			break;

		case EASE_OUT_SINE:
			easing = OutSine(t, x1, x2);
			break;

		case EASE_IN_OUT_SINE:
			easing = InOutSine(t, x1, x2);
			break;

		case EASE_OUT_IN_SINE:
			easing = OutInSine(t, x1, x2);
			break;

		case EASE_IN_QUAD:
			easing = InQuad(t, x1, x2);
			break;

		case EASE_OUT_QUAD:
			easing = OutQuad(t, x1, x2);
			break;

		case EASE_IN_OUT_QUAD:
			easing = InOutQuad(t, x1, x2);
			break;

		case EASE_OUT_IN_QUAD:
			easing = OutInQuad(t, x1, x2);
			break;

		case EASE_IN_CUBIC:
			easing = InCubic(t, x1, x2);
			break;

		case EASE_OUT_CUBIC:
			easing = OutCubic(t, x1, x2);
			break;

		case EASE_IN_OUT_CUBIC:
			easing = InOutCubic(t, x1, x2);
			break;

		case EASE_OUT_IN_CUBIC:
			easing = OutInCubic(t, x1, x2);
			break;

		case EASE_IN_QUART:
			easing = InQuart(t, x1, x2);
			break;

		case EASE_OUT_QUART:
			easing = OutQuart(t, x1, x2);
			break;

		case EASE_IN_OUT_QUART:
			easing = InOutQuart(t, x1, x2);
			break;

		case EASE_OUT_IN_QUART:
			easing = OutInQuart(t, x1, x2);
			break;

		case EASE_IN_QUINT:
			easing = InQuint(t, x1, x2);
			break;

		case EASE_OUT_QUINT:
			easing = OutQuint(t, x1, x2);
			break;

		case EASE_IN_OUT_QUINT:
			easing = InOutQuint(t, x1, x2);
			break;

		case EASE_OUT_IN_QUINT:
			easing = OutInQuint(t, x1, x2);
			break;

		case EASE_IN_EXPO:
			easing = InExpo(t, x1, x2);
			break;

		case EASE_OUT_EXPO:
			easing = OutExpo(t, x1, x2);
			break;

		case EASE_IN_OUT_EXPO:
			easing = InOutExpo(t, x1, x2);
			break;

		case EASE_OUT_IN_EXPO:
			easing = OutInExpo(t, x1, x2);
			break;

		case EASE_IN_CIRC:
			easing = InCirc(t, x1, x2);
			break;

		case EASE_OUT_CIRC:
			easing = OutCirc(t, x1, x2);
			break;

		case EASE_IN_OUT_CIRC:
			easing = InOutCirc(t, x1, x2);
			break;

		case EASE_OUT_IN_CIRC:
			easing = OutInCirc(t, x1, x2);
			break;

		case EASE_IN_BACK:
			easing = InBack(t, x1, x2);
			break;

		case EASE_OUT_BACK:
			easing = OutBack(t, x1, x2);
			break;

		case EASE_IN_OUT_BACK:
			easing = InOutBack(t, x1, x2);
			break;

		case EASE_OUT_IN_BACK:
			easing = OutInBack(t, x1, x2);
			break;

		case EASE_IN_ELASTIC:
			easing = InElastic(t, x1, x2);
			break;

		case EASE_OUT_ELASTIC:
			easing = OutElastic(t, x1, x2);
			break;

		case EASE_IN_OUT_ELASTIC:
			easing = InOutElastic(t, x1, x2);
			break;

		case EASE_OUT_IN_ELASTIC:
			easing = OutInElastic(t, x1, x2);
			break;

		case EASE_IN_BOUNCE:
			easing = InBounce(t, x1, x2);
			break;

		case EASE_OUT_BOUNCE:
			easing = OutBounce(t, x1, x2);
			break;

		case EASE_IN_OUT_BOUNCE:
			easing = InOutBounce(t, x1, x2);
			break;

		case EASE_OUT_IN_BOUNCE:
			easing = OutInBounce(t, x1, x2);
			break;

		case EASE_LINEAR:
			easing = Linear(t, x1, x2);
			break;

		default:
            easing = None(t, x1, x2);
			break;
	}

	return easing;
}

float Ease::Count2Time(int count, int countMax) {
	// 割る値が 0 なら強制的に 1.0f を返す
	if (countMax == 0) {
		return 1.0f;

	} else {
		// 時間を計算
		float t = static_cast<float>(count) / static_cast<float>(countMax);

		// 値が必ず 0.0f ～ 1.0f になるようにする
		if (t > 1.0f) {
			t = 1.0f;

		} else if (t < 0.0f) {
			t = 0.0f;
		}

		return t;
	}
}

float Ease::None(float t, float x1, float x2) {
	// t が 1.0f より下なら始点、1.0f 以上なら終点を返す
	if (t < 1.0f) {
		return x1;
	} else {
		return x2;
    }
}

float Ease::InSine(float t, float x1, float x2) {
	float easedT = 1.0f - cosf((t * M_PI) / 2.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutSine(float t, float x1, float x2) {
	float easedT = sinf((t * M_PI) / 2.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutSine(float t, float x1, float x2) {
	float easedT = -(cosf(M_PI * t) - 1.0f) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInSine(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? 0.5f * sinf(M_PI * t)
		: 1.0f - 0.5f * cosf(M_PI * (t - 0.5f)) * 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InQuad(float t, float x1, float x2) {
	float easedT = t * t;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutQuad(float t, float x1, float x2) {
	float easedT = 1.0f - (1.0f - t) * (1.0f - t);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutQuad(float t, float x1, float x2) {
	float easedT = t < 0.5f ? 2.0f * t * t
		: 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInQuad(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? 0.5f * (1.0f - (1.0f - 2.0f * t) * (1.0f - 2.0f * t))
		: 0.5f * ((2.0f * t - 1.0f) * (2.0f * t - 1.0f)) + 0.5f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InCubic(float t, float x1, float x2) {
	float easedT = t * t * t;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutCubic(float t, float x1, float x2) {
	float easedT = 1.0f - powf(1.0f - t, 3.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutCubic(float t, float x1, float x2) {
	float easedT = t < 0.5f ? 4.0f * t * t * t
		: 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInCubic(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? 0.5f * (1.0f - powf(1.0f - 2.0f * t, 3.0f))
		: 0.5f * powf(2.0f * t - 1.0f, 3.0f) + 0.5f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InQuart(float t, float x1, float x2) {
	float easedT = t * t * t * t;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutQuart(float t, float x1, float x2) {
	float easedT = 1.0f - powf(1.0f - t, 4.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutQuart(float t, float x1, float x2) {
	float easedT = t < 0.5f ? 8.0f * t * t * t * t
		: 1.0f - powf(-2.0f * t + 2.0f, 4.0f) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInQuart(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? 0.5f * (1.0f - powf(1.0f - 2.0f * t, 4.0f))
		: 0.5f * powf(2.0f * t - 1.0f, 4.0f) + 0.5f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InQuint(float t, float x1, float x2) {
	float easedT = t * t * t * t * t;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutQuint(float t, float x1, float x2) {
	float easedT = 1.0f - powf(1.0f - t, 5.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutQuint(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? 16.0f * t * t * t * t * t
		: 1.0f - powf(-2.0f * t + 2.0f, 5.0f) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInQuint(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? 0.5f * (1.0f - powf(1.0f - 2.0f * t, 5.0f))
		: 0.5f * powf(2.0f * t - 1.0f, 5.0f) + 0.5f;
	return (1.0f - easedT) * x1 + easedT * x2;
}


float Ease::InExpo(float t, float x1, float x2) {
	float easedT = t == 0.0f ? 0.0f
		: powf(2.0f, 10.0f * t - 10.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutExpo(float t, float x1, float x2) {
	float easedT = t == 1.0f ? 1.0f
		: 1.0f - powf(2.0f, -10.0f * t);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutExpo(float t, float x1, float x2) {
	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f
		: (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInExpo(float t, float x1, float x2) {
	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: t < 0.5f ? (1.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f
		: (powf(2.0f, 20.0f * (t - 0.5f) - 10.0f) + 1.0f) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InCirc(float t, float x1, float x2) {
	float easedT = 1.0f - sqrtf(1.0f - powf(t, 2.0f));
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutCirc(float t, float x1, float x2) {
	float easedT = sqrtf(1.0f - powf(t - 1.0f, 2.0f));
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutCirc(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? (1.0f - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f
		: (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInCirc(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? 0.5f * (1.0f - sqrtf(1.0f - powf(1.0f - 2.0f * t, 2.0f)))
		: 0.5f * (sqrtf(1.0f - powf(2.0f * t - 1.0f, 2.0f)) + 1.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InBack(float t, float x1, float x2) {
	static const float c1 = 1.70158f;
	static const float c3 = c1 + 1.0f;

	float easedT = c3 * t * t * t - c1 * t * t;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutBack(float t, float x1, float x2) {
	static const float c1 = 1.70158f;
	static const float c3 = c1 + 1.0f;

	float easedT = 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutBack(float t, float x1, float x2) {
	static const float c1 = 1.70158f;
	static const float c2 = c1 * 1.525f;

	float easedT = t < 0.5f
		? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
		: (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInBack(float t, float x1, float x2) {
	static const float c1 = 1.70158f;
	static const float c2 = c1 * 1.525f;

	float easedT = t < 0.5f
		? 0.5f * ((1.0f - powf(1.0f - 2.0f * t, 2.0f) * ((c2 + 1.0f) * (1.0f - 2.0f * t) - c2)))
		: 0.5f * (powf(2.0f * t - 1.0f, 2.0f) * ((c2 + 1.0f) * (2.0f * t - 1.0f) + c2) + 1.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InElastic(float t, float x1, float x2) {
	static const float c4 = (2.0f * M_PI) / 3.0f;

	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutElastic(float t, float x1, float x2) {
	static const float c4 = (2.0f * M_PI) / 3.0f;

	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutElastic(float t, float x1, float x2) {
	static const float c5 = (2.0f * M_PI) / 4.5f;

	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: t < 0.5f
		? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f
		: (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInElastic(float t, float x1, float x2) {
	static const float c5 = (2.0f * M_PI) / 4.5f;

	float easedT = t == 0.0f ? 0.0f
		: t == 1.0f ? 1.0f
		: t < 0.5f
		? 0.5f * (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5) + 1.0f)
		: 0.5f * (-powf(2.0f, 20.0f * (t - 0.5f) - 10.0f) * sinf((20.0f * (t - 0.5f) - 11.125f) * c5)) + 0.5f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InBounce(float t, float x1, float x2) {
	float easedT = 1.0f - OutBounce(1.0f - t, 0.0f, 1.0f);
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutBounce(float t, float x1, float x2) {
	static const float n1 = 7.5625f;
	static const float d1 = 2.75f;

	float easedT = 0.0f;
	if (t < 1.0f / d1) {
		easedT = n1 * t * t;

	} else if (t < 2 / d1) {
		easedT = n1 * (t -= 1.5f / d1) * t + 0.75f;

	} else if (t < 2.5 / d1) {
		easedT = n1 * (t -= 2.25f / d1) * t + 0.9375f;

	} else {
		easedT = n1 * (t -= 2.625f / d1) * t + 0.984375f;
	}

	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::InOutBounce(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? (1.0f - OutBounce(1.0f - 2.0f * t, 0.0f, 1.0f)) / 2.0f
		: (1.0f + OutBounce(2.0f * t - 1.0f, 0.0f, 1.0f)) / 2.0f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::OutInBounce(float t, float x1, float x2) {
	float easedT = t < 0.5f
		? 0.5f * OutBounce(2.0f * t, 0.0f, 1.0f)
		: 0.5f * (1.0f - OutBounce(2.0f * (1.0f - t), 0.0f, 1.0f)) + 0.5f;
	return (1.0f - easedT) * x1 + easedT * x2;
}

float Ease::Linear(float t, float x1, float x2) {
	return (1.0f - t) * x1 + t * x2;
}

} // namespace KashipanEngine