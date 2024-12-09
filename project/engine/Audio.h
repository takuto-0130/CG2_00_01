#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <fstream>
#include <wrl.h>
#include <mutex>
#include <unordered_map>
#include <array>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <Logger.h>

// WAVヘッダーの定義
struct WAVHeader {
	char riff[4];			// "RIFF"
	uint32_t size;			// ファイルサイズ
	char wave[4];			// "WAVE"
	char fmt[4];			// "fmt "
	uint32_t fmtSize;		// fmtチャンクのサイズ
	uint16_t audioFormat;	// オーディオフォーマット（1はPCM）
	uint16_t numChannels;	// チャネル数
	uint32_t sampleRate;	// サンプルレート
	uint32_t byteRate;		// バイトレート
	uint16_t blockAlign;	// ブロックアライメント
	uint16_t bitsPerSample;	// サンプルあたりのビット数
	char data[4];			// "data"
	uint32_t dataSize;		// データチャンクのサイズ
};

// 再生用コールバック
class StreamingVoiceCallback : public IXAudio2VoiceCallback {
public:
	void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override {
		std::lock_guard<std::mutex> lock(mutex);
		bufferAvailable = true;
		cv.notify_one();
	}

	void STDMETHODCALLTYPE OnBufferStart(void*) override {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
	void STDMETHODCALLTYPE OnStreamEnd() override {}
	void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
	void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}

	void WaitForBuffer() {
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [this] { return bufferAvailable; });
		bufferAvailable = false;
	}

private:
	std::mutex mutex;
	std::condition_variable cv;
	bool bufferAvailable = false;
};


// 音源の同時再生数
static const size_t kMaxPlayWave = 100;

class Audio
{
public:
	static Audio* GetInstance()
	{
		static Audio instance; // 静的ローカル変数（寿命がプログラム全体に渡る）
		return &instance;
	}

private:
	Audio() = default;
	~Audio();
	Audio(Audio&) = default;
	Audio& operator=(Audio&) = default;
public:
	void StartStreaming(const char* filename, bool isLoop = false) {
		// すでにストリーミング中の場合は終了
		if (isStreaming.load()) {
			StopStreaming();
		}
		if (isLoop) {
			isLoopStreaming.store(true);
		}

		isStreaming.store(true);

		// デタッチ可能なスレッドでストリーミングを開始
		audioThread = std::make_unique<std::thread>(&Audio::StreamAudio, this, filename);
	}

	void StopStreaming() {
        if (isStreaming.load()) {
            isStreaming.store(false); // ストリーミングを停止するフラグをセット

            if (audioThread && audioThread->joinable()) {
                audioThread->join(); // スレッドが終了するのを待つ
				Logger::Log("Stop streaming thread.\n");
            }
        }
    }

	void SetPitch(std::atomic<float>* pitch) { pitch_ = pitch; }
private:
	void StreamAudio(const char* filename);


private: // 構造体
	// チャンクヘッダー
	struct ChunkHeader {
		char id[4]; // チャンクごとのID
		int32_t size; // チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk; // "RIFF"
		char type[4]; // "WAVE"
	};

	// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; // "fmt"
		WAVEFORMATEX fmt; // 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		WAVEFORMATEX wfex; // 波形フォーマット
		BYTE* pBuffer; // バッファの先頭アドレス
		unsigned int bufferSize; // バッファのサイズ
		int playSoundLength;
	};

public:
	// 初期化
	void Initialize(const std::string& directoryPath = "Resources/");

	/**
	 * @brief 音源の停止
	 * @param resourceNum BGMのリソース番号
	 */
	void StopBGM(int resourceNum);

	/**
	 * @brief 音源のポーズ
	 * @param resourceNum BGMのリソース番号
	 */
	void PauseBGM(int resourceNum);

	/**
	 * @brief 音源の再開
	 * @param resourceNum BGMのリソース番号
	 */
	void ReStartBGM(int resourceNum);

	/**
	 * @brief 音量調整
	 * @param resourceNum BGMのリソース番号
	 */
	void SetBGMVolume(int resourceNum, float volume);

	// 音声読み込み
	void LoadWave(const char* filename);

	// 音声データ解放
	void SoundUnload(const char* filename);

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
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	// サウンドデータ格納コンテナ
	std::unordered_map<std::string, Audio::SoundData> soundDataMap;

	// 再生中データコンテナ
	std::array<IXAudio2SourceVoice*, kMaxPlayWave> pSourceVoices_ = { nullptr };

	// ストリーミング再生
	std::atomic<bool> isStreaming;
	std::atomic<bool> isLoopStreaming;
	std::unique_ptr<std::thread> audioThread;

	std::atomic<float>* pitch_;

	// サウンド格納ディレクトリ
	std::string directoryPath_;
};
