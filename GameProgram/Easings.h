#pragma once

// イージングの種類
enum EaseType {
	EASE_NONE,

	EASE_IN_SINE,
	EASE_OUT_SINE,
	EASE_IN_OUT_SINE,
	EASE_OUT_IN_SINE,

	EASE_IN_QUAD,
	EASE_OUT_QUAD,
	EASE_IN_OUT_QUAD,
	EASE_OUT_IN_QUAD,

	EASE_IN_CUBIC,
	EASE_OUT_CUBIC,
	EASE_IN_OUT_CUBIC,
	EASE_OUT_IN_CUBIC,

	EASE_IN_QUART,
	EASE_OUT_QUART,
	EASE_IN_OUT_QUART,
	EASE_OUT_IN_QUART,

	EASE_IN_QUINT,
	EASE_OUT_QUINT,
	EASE_IN_OUT_QUINT,
	EASE_OUT_IN_QUINT,

	EASE_IN_EXPO,
	EASE_OUT_EXPO,
	EASE_IN_OUT_EXPO,
	EASE_OUT_IN_EXPO,

	EASE_IN_CIRC,
	EASE_OUT_CIRC,
	EASE_IN_OUT_CIRC,
	EASE_OUT_IN_CIRC,

	EASE_IN_BACK,
	EASE_OUT_BACK,
	EASE_IN_OUT_BACK,
	EASE_OUT_IN_BACK,

	EASE_IN_ELASTIC,
	EASE_OUT_ELASTIC,
	EASE_IN_OUT_ELASTIC,
	EASE_OUT_IN_ELASTIC,

	EASE_IN_BOUNCE,
	EASE_OUT_BOUNCE,
	EASE_IN_OUT_BOUNCE,
	EASE_OUT_IN_BOUNCE,

	EASE_LINER,

	// イージングの総数
	EASINGS
};

class Ease {
public:
	// イージングのenum名
	static const char *easeName_[EASINGS];

	/// <summary>
	/// イージングのちょっとめんどくさい処理を勝手にやってくれる関数
	/// </summary>
	/// <param name="count">現在のカウント</param>
	/// <param name="countMax">カウントの最大値</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <param name="easeingType">使うイージングの種類("EASE_"から始まる定数)</param>
	/// <param name="scale">イージングの動きの倍率。値を大きくすればするほどイージングの動きにキレが出る。</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float Auto(int count, int countMax, float x1, float x2, int easeType = EASE_NONE, float scale = 1.0f);

	/// <summary>
	/// カウントをイージング用の時間(0.0 ～ 1.0)に変換
	/// </summary>
	/// <param name="count">現在のカウント</param>
	/// <param name="countMax">カウントの最大値</param>
	/// <returns>0.0f ～ 1.0f の時間</returns>
	static float Count2Time(int count, int countMax);

	/// <summary>
	/// イージング 加速(Sine)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InSine(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Sine)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutSine(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Sine)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutSine(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Sine)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInSine(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Quad)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InQuad(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Quad)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutQuad(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Quad)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutQuad(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Quad)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInQuad(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Cubic)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InCubic(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Cubic)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutCubic(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Cubic)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutCubic(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Cubic)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInCubic(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Quart)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InQuart(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Quart)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutQuart(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Quart)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutQuart(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Quart)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInQuart(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Quint)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InQuint(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Quint)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutQuint(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Quint)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutQuint(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Quint)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInQuint(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Expo)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InExpo(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Expo)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutExpo(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Expo)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutExpo(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Expo)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInExpo(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Circ)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InCirc(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Circ)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutCirc(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Circ)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutCirc(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Circ)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInCirc(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Back)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InBack(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Back)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutBack(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Back)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutBack(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Back)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInBack(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Elastic)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InElastic(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Elastic)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutElastic(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Elastic)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutElastic(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Elastic)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInElastic(float t, float x1, float x2);

	/// <summary>
	/// イージング 加速(Bounce)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InBounce(float t, float x1, float x2);

	/// <summary>
	/// イージング 減速(Bounce)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutBounce(float t, float x1, float x2);

	/// <summary>
	/// イージング 加減速(Bounce)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float InOutBounce(float t, float x1, float x2);

	/// <summary>
	/// イージング 減加速(Bounce)
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float OutInBounce(float t, float x1, float x2);

	/// <summary>
	/// イージング 等速直線運動
	/// </summary>
	/// <param name="t">時間</param>
	/// <param name="x1">イージングの始点</param>
	/// <param name="x2">イージングの終点</param>
	/// <returns>x1 ～ x2 の間でイージングした値</returns>
	static float Liner(float t, float x1, float x2);
};