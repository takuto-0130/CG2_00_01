#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <fstream>
#include <wrl.h>
#include <unordered_map>
#include <array>
#include <vector>
#include <Logger.h>
#include <xaudio2fx.h>
#include <cassert>
#include <queue>

#include "Audio.h"
#include "StreamingAudio.h"
#include "AudioData.h"


class AudioManager
{
public:
    static AudioManager* GetInstance() {
        static AudioManager instance;
        return &instance;
    }

public:

    void Initialize(const std::string& directoryPath = "Resources/");

    void LoadWave(const char* filename);

    void UnloadWave(const char* filename);

    /**
     * @brief マスター音量の設定
     * @param volume 設定したい主音量(0~1を推奨)
     */
    void SetMasterVolume(float volume) { masterVoice_->SetVolume(volume); }


public: 
    ////////////////////////////
    /*** ▼▼ 通常の音声再生 ▼▼ ***/
    ////////////////////////////

    /**
     * @brief 音声を再生する
     * @param filename 再生する音声ファイルの名前
     * @param audioType 音声の種類
     * @param isLoop ループ再生するかどうか（デフォルト: false）
     * @return AudioID 再生された音声の識別情報
     */
    AudioID PlayWave(const std::string& filename, const AudioType& audioType, bool isLoop = false);

    void StopSound(AudioID id) { audio_[id.audioType]->StopAudio(id.resourceNum); }

    void PauseSound(AudioID id) { audio_[id.audioType]->PauseAudio(id.resourceNum); }

    void RestartSound(AudioID id) { audio_[id.audioType]->ReStartAudio(id.resourceNum); }

    void SetSoundVolume(AudioID id, float volume) { audio_[id.audioType]->SetAudioVolume(id.resourceNum, volume); }

    void SetSubmixVolume(const AudioType& audioType, float volume) { audio_[audioType]->SetSubmixVolume(volume); }

    ////////////////////////////
    /*** ▲▲ 通常の音声再生 ▲▲ ***/
    ////////////////////////////


public: 
    ////////////////////////////////////////////////////
    /*** ▼▼ ストリーミング再生 (最大3インスタンス制限) ▼▼ ***/
    ////////////////////////////////////////////////////

    void StartStreaming(const std::string& filename, bool isLoop = false);

    void StopStreaming(const std::string& filename);

private:
    void RemoveOldestStreaming();

    ///////////////////////////////
    /*** ▲▲ ストリーミング再生 ▲▲ ***/
    ///////////////////////////////


private:
    AudioManager() = default;
    ~AudioManager();
    AudioManager(AudioManager&) = delete;
    AudioManager& operator=(AudioManager&) = delete;

private:

    std::array<std::unique_ptr<Audio>, static_cast<size_t>(AudioType::kTypeNum)> audio_;
    std::string directoryPath_;
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	// サウンドデータ格納コンテナ
	std::unordered_map<std::string, SoundData> soundDataMap_;


    std::unordered_map<std::string, std::unique_ptr<StreamingAudio>> streamingAudioMap_;
    std::queue<std::string> streamingQueue_;  // 古い順に管理
    static constexpr int maxStreamingInstances_ = 3;  // 最大3つまで
};

