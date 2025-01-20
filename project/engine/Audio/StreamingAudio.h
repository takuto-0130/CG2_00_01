#pragma once
#include <xaudio2.h>
#include <string>
#include <fstream>
#include <mutex>
#include <array>
#include <vector>
#include <thread>
#include <atomic>
#include <Logger.h>
#include <xaudio2fx.h>

class Audio;

class StreamingAudio
{
private:
	// AudioのみがStreamingAudioにアクセスできるようにする
	friend class Audio;

	StreamingAudio(IXAudio2* xAudio2, const std::string& directoryPath) : xAudio2_(xAudio2), directoryPath_(directoryPath), id_(nextID_++) {
	}

	~StreamingAudio();

private:
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

public:
	void StartStreaming(const char* filename, bool isLoop = false);

	void StopStreaming();

	void SetPitch(float pitch);

	/**
	 * @brief エフェクトチェーンの設定
	 * @param effectsNum 設定したいエフェクトの数
	 * @param effects エフェクトの設定の配列
	 */
	void SetEffectChain(uint32_t effectsNum, XAUDIO2_EFFECT_DESCRIPTOR* effects) {
		effectChain.EffectCount = effectsNum;
		effectChain.pEffectDescriptors = effects;
	}

	/**
	 * @brief エフェクトチェーンの適用
	 */
	void ApplyEffectChain();

	/**
	 * @brief エフェクトの設定と有効化（現状一つのみ対応）
	 * @param parameters プリセット、"XAUDIO2FX_I3DL2_PRESET_~" を入れるか自力で設定
	 */
	void SetEffect(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS parameters = {});

	void DisableEffect();

private:

	void StreamAudio(const char* filename);

	/**
	 * @brief エフェクトチェーンの初期化
	 */
	void InitEffectChain();



	// ファイルからデータを読み込む関数
	bool ReadAudioData(std::ifstream& file, std::vector<BYTE>& buffer);

	// WAVファイルのヘッダーを読み込む
	bool ReadWavHeader(std::string filename, WAVHeader& header);

public:
	uint32_t GetId() { return id_; }

private:
	IXAudio2* xAudio2_;

	// サウンド格納ディレクトリ
	std::string directoryPath_;

	// ID
	static uint32_t nextID_;
	uint32_t id_;


	// ストリーミング再生
	std::atomic<bool> isStreaming;
	std::atomic<bool> isLoopStreaming;
	std::unique_ptr<std::thread> audioThread;
	IXAudio2SourceVoice* streamVoice = nullptr;
	std::vector<std::vector<BYTE>> audioBuffers;
	size_t BUFFER_SIZE;
	XAUDIO2FX_REVERB_PARAMETERS reverbParameters = {};
	XAUDIO2_EFFECT_CHAIN effectChain = {};
	XAUDIO2_EFFECT_DESCRIPTOR effect[1] = {};
};

