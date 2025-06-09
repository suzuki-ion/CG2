#define XAUDIO2_HELPER_FUNCTIONS

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <xaudio2.h>
#include <wrl.h>
#include <cassert>
#include <fstream>
#include <vector>
#include <format>
#include <map>

#include "Sound.h"
#include "Common/Logs.h"

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace KashipanEngine {

namespace {

//==================================================
// 音声用構造体
//==================================================

/// @brief 音声データ
struct SoundData {
    /// @brief 波形フォーマット
    WAVEFORMATEX wfex;
    /// @brief バッファの先頭アドレス
    BYTE *pBuffer;
    /// @brief バッファのサイズ
    unsigned int bufferSize;
    /// @brief 音声の再生用オブジェクト
    IXAudio2SourceVoice *pSourceVoice;
};

//==================================================
// 音声再生用オブジェクト
//==================================================

/// @brief XAudio2オブジェクト
Microsoft::WRL::ComPtr<IXAudio2> sXaudio2;
/// @brief マスターボイス
IXAudio2MasteringVoice *sMasterVoice;
/// @brief 音声データのリスト
std::vector<SoundData> sSoundData;

//==================================================
// テーブル
//==================================================

/// @brief ピッチテーブル
std::map<std::string, float> sPitchTable = {
    {"C0", -48.0f}, {"C#0", -47.0f}, {"D0", -46.0f}, {"D#0", -45.0f}, {"E0", -44.0f}, {"F0", -43.0f}, {"F#0", -42.0f}, {"G0", -41.0f}, {"G#0", -40.0f}, {"A0", -39.0f}, {"A#0", -38.0f}, {"B0", -37.0f},
    {"C1", -36.0f}, {"C#1", -35.0f}, {"D1", -34.0f}, {"D#1", -33.0f}, {"E1", -32.0f}, {"F1", -31.0f}, {"F#1", -30.0f}, {"G1", -29.0f}, {"G#1", -28.0f}, {"A1", -27.0f}, {"A#1", -26.0f}, {"B1", -25.0f},
    {"C2", -24.0f}, {"C#2", -23.0f}, {"D2", -22.0f}, {"D#2", -21.0f}, {"E2", -20.0f}, {"F2", -19.0f}, {"F#2", -18.0f}, {"G2", -17.0f}, {"G#2", -16.0f}, {"A2", -15.0f}, {"A#2", -14.0f}, {"B2", -13.0f},
    {"C3", -12.0f}, {"C#3", -11.0f}, {"D3", -10.0f}, {"D#3", -9.0f},  {"E3", -8.0f},  {"F3", -7.0f},  {"F#3", -6.0f},  {"G3", -5.0f},  {"G#3", -4.0f},  {"A3", -3.0f},  {"A#3", -2.0f},  {"B3", -1.0f },
    {"C4", 0.0f },  {"C#4", 1.0f },  {"D4", 2.0f },  {"D#4", 3.0f },  {"E4", 4.0f },  {"F4", 5.0f },  {"F#4", 6.0f },  {"G4", 7.0f },  {"G#4", 8.0f },  {"A4", 9.0f },  {"A#4", 10.0f},  {"B4", 11.0f },
    {"C5", 12.0f},  {"C#5", 13.0f},  {"D5", 14.0f},  {"D#5", 15.0f},  {"E5", 16.0f},  {"F5", 17.0f},  {"F#5", 18.0f},  {"G5", 19.0f},  {"G#5", 20.0f},  {"A5", 21.0f},  {"A#5", 22.0f},  {"B5", 23.0f },
    {"C6", 24.0f},  {"C#6", 25.0f},  {"D6", 26.0f},  {"D#6", 27.0f},  {"E6", 28.0f},  {"F6", 29.0f},  {"F#6", 30.0f},  {"G6", 31.0f},  {"G#6", 32.0f},  {"A6", 33.0f},  {"A#6", 34.0f},  {"B6", 35.0f },
    {"C7", 36.0f},  {"C#7", 37.0f},  {"D7", 38.0f},  {"D#7", 39.0f},  {"E7", 40.0f},  {"F7", 41.0f},  {"F#7", 42.0f},  {"G7", 43.0f},  {"G#7", 44.0f},  {"A7", 45.0f},  {"A#7", 46.0f},  {"B7", 47.0f },
    {"C8", 48.0f},  {"C#8", 49.0f},  {"D8", 50.0f},  {"D#8", 51.0f},  {"E8", 52.0f},  {"F8", 53.0f},  {"F#8", 54.0f},  {"G8", 55.0f},  {"G#8", 56.0f},  {"A8", 57.0f},  {"A#8", 58.0f},  {"B8", 59.0f },
    {"C9", 60.0f},  {"C#9", 61.0f},  {"D9", 62.0f},  {"D#9", 63.0f},  {"E9", 64.0f},  {"F9", 65.0f},  {"F#9", 66.0f},  {"G9", 67.0f},  {"G#9", 68.0f},  {"A9", 69.0f},  {"A#9", 70.0f},  {"B9", 71.0f },
    {"C10", 72.0f}
};

} // namespace

void Sound::Initialize() {
    // XAudio2の初期化
    HRESULT hr = XAudio2Create(&sXaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        assert(SUCCEEDED(hr));
    }
    // マスターボイスの作成
    hr = sXaudio2->CreateMasteringVoice(&sMasterVoice);
    if (FAILED(hr)) {
        assert(SUCCEEDED(hr));
    }

    // Media Foundationの初期化
    hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
        Log("Failed to initialize Media Foundation.", kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }

    Log("XAudio2 initialized successfully.", kLogLevelFlagInfo);
}

void Sound::Finalize() {
    // Media Foundationの終了
    HRESULT hr = MFShutdown();
    if (FAILED(hr)) {
        Log("Failed to shutdown Media Foundation.", kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }

    // XAudio2の解放
    sXaudio2.Reset();
    // 音声データの解放
    for (size_t i = 0; i < sSoundData.size(); ++i) {
        Unload(static_cast<int>(i));
    }

    Log("XAudio2 finalized successfully.", kLogLevelFlagInfo);
}

int Sound::Load(const std::string &filePath) {
    //==================================================
    // ソースリーダーの作成
    //==================================================

    Microsoft::WRL::ComPtr<IMFSourceReader> pReader;
    HRESULT hr = MFCreateSourceReaderFromURL(std::wstring(filePath.begin(), filePath.end()).c_str(), nullptr, &pReader);
    if (FAILED(hr)) {
        Log("Failed to create source reader for: " + filePath, kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }

    //==================================================
    // メディアタイプの取得
    //==================================================

    Microsoft::WRL::ComPtr<IMFMediaType> pType;
    hr = MFCreateMediaType(&pType);
    if (FAILED(hr)) {
        Log("Failed to create media type for: " + filePath, kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }
    hr = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    if (FAILED(hr)) {
        Log("Failed to set major type for: " + filePath, kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }
    hr = pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    if (FAILED(hr)) {
        Log("Failed to set subtype for: " + filePath, kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }
    hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pType.Get());
    if (FAILED(hr)) {
        Log("Failed to set current media type for: " + filePath, kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }

    pType.Reset();
    pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pType);

    //==================================================
    // オーディオデータ形式の作成
    //==================================================

    WAVEFORMATEX *pFormat = nullptr;
    hr = MFCreateWaveFormatExFromMFMediaType(pType.Get(), &pFormat, nullptr);
    if (FAILED(hr)) {
        Log("Failed to create wave format from media type for: " + filePath, kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }

    //==================================================
    // 音声データの読み込み
    //==================================================

    std::vector<BYTE> mediaData;
    while (true) {
        IMFSample *pSample = nullptr;
        DWORD dwStreamFlags = 0;
        hr = pReader->ReadSample(
            MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            nullptr,
            &dwStreamFlags,
            nullptr,
            &pSample
        );
        if (FAILED(hr)) {
            Log("Failed to read sample from: " + filePath, kLogLevelFlagError);
            assert(SUCCEEDED(hr));
        }

        if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
            // すべてのサンプルを読み終えた場合
            break;
        }

        IMFMediaBuffer *pMediaBuffer = nullptr;
        hr = pSample->ConvertToContiguousBuffer(&pMediaBuffer);
        if (FAILED(hr)) {
            Log("Failed to convert sample to contiguous buffer for: " + filePath, kLogLevelFlagError);
            assert(SUCCEEDED(hr));
        }

        BYTE *pBuffer = nullptr;
        DWORD cbCurrentLength = 0;
        hr = pMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);
        if (FAILED(hr)) {
            Log("Failed to lock media buffer for: " + filePath, kLogLevelFlagError);
            assert(SUCCEEDED(hr));
        }

        mediaData.resize(mediaData.size() + cbCurrentLength);
        std::memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

        hr = pMediaBuffer->Unlock();
        if (FAILED(hr)) {
            Log("Failed to unlock media buffer for: " + filePath, kLogLevelFlagError);
            assert(SUCCEEDED(hr));
        }

        pMediaBuffer->Release();
        pSample->Release();
    }

    //==================================================
    // 音声データのバッファを作成
    //==================================================

    SoundData data;
    data.wfex = *pFormat;
    data.bufferSize = sizeof(BYTE) * static_cast<unsigned int>(mediaData.size());
    data.pBuffer = new BYTE[mediaData.size()];
    std::memcpy(data.pBuffer, mediaData.data(), mediaData.size());
    sSoundData.push_back(data);

    // 読み込んだ音声ファイルのログ
    Log(std::format("Load Sound: {} ({} bytes)", filePath, data.bufferSize), kLogLevelFlagInfo);
    // 音声データのインデックスを返す
    return static_cast<int>(sSoundData.size() - 1);
}

void Sound::Unload(int index) {
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return;
    }
    // 音声データの解放
    delete[] sSoundData[index].pBuffer;
    sSoundData[index].pBuffer = nullptr;
    sSoundData[index].bufferSize = 0;
    sSoundData[index].wfex = {};
}

void Sound::Play(int index, float volume, float pitch, bool loop) {
    HRESULT hr;
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return;
    }

    // 波形フォーマットを元にSourceVoiceの作成
    hr = sXaudio2->CreateSourceVoice(
        &sSoundData[index].pSourceVoice,
        &sSoundData[index].wfex);
    if (FAILED(hr)) {
        Log("Failed to create source voice: " + std::to_string(index), kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }

    // 再生する音声データの設定
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = sSoundData[index].bufferSize;
    buffer.pAudioData = sSoundData[index].pBuffer;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

    // 音声データの再生
    hr = sSoundData[index].pSourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        Log("Failed to submit source buffer: " + std::to_string(index), kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }
    // 音声データの再生開始
    hr = sSoundData[index].pSourceVoice->Start();
    if (FAILED(hr)) {
        Log("Failed to start source voice: " + std::to_string(index), kLogLevelFlagError);
        assert(SUCCEEDED(hr));
    }
    // 音声データのボリューム設定
    sSoundData[index].pSourceVoice->SetVolume(volume);
    // 音声データのピッチ設定
    sSoundData[index].pSourceVoice->SetFrequencyRatio(XAudio2SemitonesToFrequencyRatio(pitch));
}

void Sound::Stop(int index) {
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return;
    }

    // 音声データの停止
    if (sSoundData[index].pSourceVoice) {
        sSoundData[index].pSourceVoice->Stop();
        sSoundData[index].pSourceVoice->DestroyVoice();
        sSoundData[index].pSourceVoice = nullptr;
    }
}

void Sound::StopAll() {
    for (size_t i = 0; i < sSoundData.size(); ++i) {
        Stop(static_cast<int>(i));
    }
}

void Sound::Pause(int index) {
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return;
    }
    // 音声データの一時停止
    if (sSoundData[index].pSourceVoice) {
        sSoundData[index].pSourceVoice->Stop();
    }
}

void Sound::Resume(int index) {
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return;
    }
    // 音声データの再開
    if (sSoundData[index].pSourceVoice) {
        sSoundData[index].pSourceVoice->Start();
    }
}

bool Sound::IsPlaying(int index) {
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return false;
    }
    // 音声データの再生状態を取得
    XAUDIO2_VOICE_STATE state;
    sSoundData[index].pSourceVoice->GetState(&state);
    return state.BuffersQueued > 0;
}

void Sound::SetVolume(int index, float volume) {
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return;
    }
    // 音声データのボリューム設定
    if (sSoundData[index].pSourceVoice) {
        sSoundData[index].pSourceVoice->SetVolume(volume);
    }
}

void Sound::SetPitch(int index, float pitch) {
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return;
    }
    // 音声データのピッチ設定
    if (sSoundData[index].pSourceVoice) {
        sSoundData[index].pSourceVoice->SetFrequencyRatio(XAudio2SemitonesToFrequencyRatio(pitch));
    }
}

void Sound::SetPitch(int index, char *pitch) {
    if (index < 0 || index >= static_cast<int>(sSoundData.size())) {
        Log("Invalid sound index: " + std::to_string(index), kLogLevelFlagError);
        return;
    }
    // 音声データのピッチ設定
    if (sSoundData[index].pSourceVoice) {
        auto it = sPitchTable.find(pitch);
        if (it != sPitchTable.end()) {
            sSoundData[index].pSourceVoice->SetFrequencyRatio(XAudio2SemitonesToFrequencyRatio(it->second));
        } else {
            Log("Invalid pitch name: " + std::string(pitch), kLogLevelFlagError);
        }
    }
}

} // namespace KashipanEngine

