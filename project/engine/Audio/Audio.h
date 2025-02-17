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

// 音源の同時再生数
static const size_t kMaxPlayWave = 50;

class Audio
{
public: // 構造体
	// 音声データ
	struct SoundData {
		WAVEFORMATEX wfex;			// 波形フォーマット
		BYTE* pBuffer;				// バッファの先頭アドレス
		unsigned int bufferSize;	// バッファのサイズ
		int playSoundLength;
	};

	// チャンクヘッダー
	struct ChunkHeader {
		char id[4];					// チャンクごとのID
		int32_t size;				// チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk;			// "RIFF"
		char type[4];				// "WAVE"
	};

	// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk;			// "fmt"
		WAVEFORMATEX fmt;			// 波形フォーマット
	};

public:
	Audio(std::unordered_map<std::string, SoundData>& soundDataMap)
		: soundDataMap_(soundDataMap) {
	}
	~Audio();

	// 初期化
	void Initialize(Microsoft::WRL::ComPtr<IXAudio2> xAudio2, const std::string& directoryPath = "Resources/");

	/**
	 * @brief 音源の停止
	 * @param resourceNum BGMのリソース番号
	 */
	void StopAudio(int resourceNum);

	/**
	 * @brief 音源のポーズ
	 * @param resourceNum BGMのリソース番号
	 */
	void PauseAudio(int resourceNum);

	/**
	 * @brief 音源の再開
	 * @param resourceNum BGMのリソース番号
	 */
	void ReStartAudio(int resourceNum);

	/**
	 * @brief 音量調整
	 * @param resourceNum BGMのリソース番号
	 */
	void SetAudioVolume(int resourceNum, float volume);

	void SetSubmixVolume(float volume)
	{
		if (submixVoice_) {
			submixVoice_->SetVolume(volume);
		}
	}

	/**
	 * @brief 音源の再生
	 * @param soundData 音源データ
	 * @param isLoop ループするか　default : false
	 * @return int BGMのリソース番号
	 */
	int PlayWave(const char* filename, const bool isLoop = false);

private:
	// 利用可能なソースボイスを検索
	int SearchSourceVoice(IXAudio2SourceVoice** sourceVoices);

	XAUDIO2_BUFFER SetBuffer(bool loop, const SoundData& sound);

private:
	// マネージャーに移す
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	// サウンドデータ格納コンテナ
	std::unordered_map<std::string, Audio::SoundData>& soundDataMap_;
	//

	// submixVoice
	IXAudio2SubmixVoice* submixVoice_;

	// 再生中データコンテナ
	std::array<IXAudio2SourceVoice*, kMaxPlayWave> pSourceVoices_ = { nullptr };

	// サウンド格納ディレクトリ
	std::string directoryPath_;
};
