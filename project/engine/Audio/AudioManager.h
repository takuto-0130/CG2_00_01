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

#include "Audio.h"


class AudioManager
{
public:
    static AudioManager* GetInstance() {
        static AudioManager instance;
        return &instance;
    }

public:
    /**
     * @enum AudioType
     * @brief オーディオの種類を表す列挙型
     */
    enum AudioType {
        kBGM,           ///< BGM（背景音楽）
        kGameSE,        ///< ゲーム内のSE（攻撃音など）
        kUISE,          ///< UIのSE（ボタン音など）
        kAmbientSound,  ///< 環境音（川の流れなど）
        kTypeNum        ///< オーディオタイプの総数（新しい種類を追加する場合はこの前に定義）
    };

    /**
     * @brief オーディオの識別情報
     */
    struct AudioID {
        int resourceNum;		///< リソース番号
        AudioType audioType;	///< 音声の種類
    };

    void Initialize(const std::string& directoryPath = "Resources/") {
        directoryPath_ = directoryPath;
        HRESULT result;
        // インスタンスの生成
        result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(result)) {
            Logger::Log("Failed to create xAudio2 instance");
            return;
        }
        // マスターボイスの生成
        result = xAudio2_->CreateMasteringVoice(&masterVoice_);
        if (FAILED(result)) {
            Logger::Log("Failed to create master voice");
            return;
        }

        // 各要素を std::make_unique で初期化
        for (size_t i = 0; i < kTypeNum; ++i) {
            audio_[i] = std::make_unique<Audio>(soundDataMap_);
            audio_[i]->Initialize(xAudio2_, directoryPath_);
        }
    }

	void LoadWave(const char* filename)
	{
		//HRESULT result;
		if (soundDataMap_.count(filename)) {
			Logger::Log("The file is not loaded or unloaded");
			// キーが存在する場合、処理を中断
			return;
		}

		// ファイル入力streamのインスタンス
		std::ifstream file;
		std::string filePath = directoryPath_;
		filePath += filename;
		filePath += ".wav";
		file.open(filePath, std::ios_base::binary);
		if (!file.is_open()) {
			Logger::Log("Failed to file open");
			return;
		}

		// .wavデータ読み込み
		// RIFFヘッダーの読み込み
		Audio::RiffHeader riff;
		file.read((char*)&riff, sizeof(riff));

		// ファイルがRIFFかチェック
		if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
			assert(0);
		}
		// タイプがWAVEがチェック
		if (strncmp(riff.type, "WAVE", 4) != 0) {
			assert(0);
		}
		// Formatチャンクの読み込み
		Audio::FormatChunk format = {};
		// チャンクヘッダーの確認
		file.read((char*)&format, sizeof(Audio::ChunkHeader));
		if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
			assert(0);
		}
		// チャンク本体の読み込み
		assert(format.chunk.size <= sizeof(format.fmt));
		file.read((char*)&format.fmt, format.chunk.size);

		// Dataチャンクの読み込み
		Audio::ChunkHeader data;
		file.read((char*)&data, sizeof(data));
		// JUNKチャンクを検出した場合
		if (strncmp(data.id, "JUNK", 4) == 0) {
			// 読み取り位置をJUNKチャンクの終わりまで進める
			file.seekg(data.size, std::ios_base::cur);
			// 再読み込み
			file.read((char*)&data, sizeof(data));
		}
		// LISTチャンクを検出した場合
		if (strncmp(data.id, "LIST", 4) == 0) {
			// 読み取り位置をLISTチャンクの終わりまで進める
			file.seekg(data.size, std::ios_base::cur);
			// 再読み込み
			file.read((char*)&data, sizeof(data));
		}
		// INFOISFTチャンクを検出した場合
		if (strncmp(data.id, "INFOISFT", 8) == 0) {
			// 読み取り位置をINFOISFTチャンクの終わりまで進める
			file.seekg(data.size, std::ios_base::cur);
			// 再読み込み
			file.read((char*)&data, sizeof(data));
		}

		if (strncmp(data.id, "data", 4) != 0) {
			assert(0);
		}

		// Dataチャンクのデータ部 (波形のデータ) の読み込み
		char* pBuffer = new char[data.size];
		file.read(pBuffer, data.size);

		// ファイルクローズ
		file.close();

		// SoundDataの生成
		Audio::SoundData soundData = {};

		soundData.wfex = format.fmt;
		soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
		soundData.bufferSize = data.size;
		soundData.playSoundLength = data.size / format.fmt.nBlockAlign;

		soundDataMap_[filename] = soundData;
	}

	void UnloadWave(const char* filename)
	{
		auto it = soundDataMap_.find(filename);
		if (it != soundDataMap_.end()) {
			delete[] it->second.pBuffer;
			soundDataMap_.erase(it);
		}
	}

    // マスター音量の設定
    void SetMasterVolume(float volume) { masterVoice_->SetVolume(volume); }

    /*** ▼▼ 通常の音声再生 ▼▼ ***/

    /**
     * @brief 音声を再生する
     * @param filename 再生する音声ファイルの名前
     * @param audioType 音声の種類
     * @param isLoop ループ再生するかどうか（デフォルト: false）
     * @return AudioID 再生された音声の識別情報
     */
    AudioID PlayWave(const std::string& filename, const AudioType& audioType,bool isLoop = false) {
        AudioID id;
        id.audioType = audioType;
        id.resourceNum = audio_[audioType]->PlayWave(filename.c_str(), isLoop);
        return id;
    }

    void StopSound(AudioID id) { audio_[id.audioType]->StopAudio(id.resourceNum); }

    void PauseSound(AudioID id) { audio_[id.audioType]->PauseAudio(id.resourceNum); }

    void RestartSound(AudioID id) { audio_[id.audioType]->ReStartAudio(id.resourceNum); }

    void SetSoundVolume(AudioID id, float volume) { audio_[id.audioType]->SetAudioVolume(id.resourceNum, volume); }

    void SetSubmixVolume(const AudioType& audioType, float volume) { audio_[audioType]->SetSubmixVolume(volume); }

private:
    AudioManager() = default;
    ~AudioManager() {
		for (auto& audio : audio_) {
			audio.reset(); // 明示的に解放
		}
		if (masterVoice_) {
			masterVoice_->DestroyVoice();
			masterVoice_ = nullptr;
		}
		xAudio2_.Reset();
	}
    AudioManager(AudioManager&) = delete;
    AudioManager& operator=(AudioManager&) = delete;

private:

    std::array<std::unique_ptr<Audio>, static_cast<size_t>(kTypeNum)> audio_;
    std::string directoryPath_;
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	// サウンドデータ格納コンテナ
	std::unordered_map<std::string, Audio::SoundData> soundDataMap_;
};

