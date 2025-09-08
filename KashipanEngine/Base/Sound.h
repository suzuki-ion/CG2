#pragma once

namespace KashipanEngine {

class Sound {
public:
    /// @brief 初期化処理
    static void Initialize();

    /// @brief 終了処理
    static void Finalize();

    /// @brief 音声ファイルを読み込む
    /// @param filePath 音声ファイルのパス
    /// @param soundName 音声の名前(設定しなかった場合はファイルパスになる)
    /// @return 音声データへのインデックス
    static int Load(const std::string &filePath, const std::string &soundName = "");

    /// @brief Jsonファイルからの音声の一括読み込み
    /// @param jsonFilePath Jsonファイルのパス
    static void LoadFromJson(const std::string &jsonFilePath);

    /// @brief 音声データへのインデックスを取得する
    /// @param soundName 音声の名前
    /// @return 音声データへのインデックス。 見つからなかった場合は0を返す
    static int FindIndex(const std::string &soundName);

    /// @brief 音声データをアンロードする
    /// @param index 音声データのインデックス
    static void Unload(int index);

    /// @brief 音声を再生する
    /// @param index 音声データのインデックス
    /// @param volume ボリューム(0.0f ~ 1.0f)
    /// @param pitch ピッチ(1.0fで半音上がる)
    /// @param loop ループ再生するかどうか
    static void Play(int index, float volume = 1.0f, float pitch = 0.0f, bool loop = false);

    /// @brief 音声を停止する
    /// @param index 音声データのインデックス
    static void Stop(int index);

    /// @brief 全ての音声を停止する
    static void StopAll();

    /// @brief 音声を一時停止する
    /// @param index 音声データのインデックス
    static void Pause(int index);

    /// @brief 音声を再開する
    /// @param index 音声データのインデックス
    static void Resume(int index);

    /// @brief 音声の再生状態を取得する
    /// @param index 音声データのインデックス
    static bool IsPlaying(int index);

    /// @brief 音声のボリュームを設定する
    /// @param index 音声データのインデックス
    /// @param volume ボリューム(0.0f ~ 1.0f)
    static void SetVolume(int index, float volume);

    /// @brief 音声のピッチを設定する
    /// @param index 音声データのインデックス
    /// @param pitch ピッチ(1.0fで半音上がる)
    static void SetPitch(int index, float pitch);

    /// @brief 音声のピッチを設定する
    /// @param index 音声データのインデックス
    /// @param pitch ピッチ名
    static void SetPitch(int index, char *pitch);

};

} // namespace KashipanEngine