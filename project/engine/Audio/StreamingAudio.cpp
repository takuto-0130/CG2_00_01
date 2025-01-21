#include "StreamingAudio.h"

uint32_t StreamingAudio::nextID_ = 0;

StreamingAudio::~StreamingAudio()
{
	StopStreaming();
	// BGMリソースの解放
	if (streamVoice != nullptr) {
		streamVoice->DestroyVoice();
		streamVoice = nullptr;
	}
}

void StreamingAudio::StartStreaming(const char* filename, bool isLoop)
{
	// すでにストリーミング中の場合は終了
	if (isStreaming.load()) {
		StopStreaming();
	}
	if (isLoop) {
		isLoopStreaming.store(true);
	}

	isStreaming.store(true);

	// デタッチ可能なスレッドでストリーミングを開始
	audioThread = std::make_unique<std::thread>(&StreamingAudio::StreamAudio, this, filename);
}

void StreamingAudio::StopStreaming()
{
	if (isStreaming.load()) {
		isStreaming.store(false); // ストリーミングを停止するフラグをセット

		if (audioThread && audioThread->joinable()) {
			audioThread->join(); // スレッドが終了するのを待つ
			Logger::Log("Stop streaming thread.\n");
		}
	}
}

void StreamingAudio::SetPitch(float pitch)
{
	if (streamVoice)
	{
		streamVoice->SetFrequencyRatio(pitch);
	}
	else {
		Logger::Log("UnInitialized streamVoice.");
	}
}

void StreamingAudio::ApplyEffectChain()
{
	if (streamVoice) {
		// エフェクトチェーンの適用
		if (FAILED(streamVoice->SetEffectChain(&effectChain)))
		{
			Logger::Log("Failed to set effect chain.");
		}
	}
	else {
		Logger::Log("UnInitialized streamVoice.");
	}
}

void StreamingAudio::SetEffect(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS parameters)
{
	if (streamVoice)
	{
		ReverbConvertI3DL2ToNative(&parameters, &reverbParameters);
		if (FAILED(streamVoice->SetEffectParameters(0, &reverbParameters, sizeof(reverbParameters)))) {
			Logger::Log("Failed to set effect parameters.");
		}
		streamVoice->EnableEffect(0);
	}
	else {
		Logger::Log("UnInitialized streamVoice.");
	}
}

void StreamingAudio::DisableEffect()
{
	if (streamVoice)
	{
		streamVoice->DisableEffect(0);
	}
}

void StreamingAudio::StreamAudio(const char* filename)
{
	std::string filePath = directoryPath_;
	filePath += filename;

	// WAVヘッダーの読み込み
	WAVHeader header;
	if (!ReadWavHeader(filePath, header)) {
		Logger::Log("Error reading WAV header.\n");
		return;
	}

	// WAVEFORMATEXの設定
	WAVEFORMATEX waveFormat = {};
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = header.numChannels;
	waveFormat.nSamplesPerSec = header.sampleRate;
	waveFormat.wBitsPerSample = header.bitsPerSample;
	waveFormat.nBlockAlign = header.numChannels * (header.bitsPerSample / 8);
	waveFormat.nAvgBytesPerSec = header.sampleRate * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0; // PCMでは0

	// ストリーミング用のファイルを開く
	std::ifstream audioFile(filePath, std::ios::binary);
	if (!audioFile) {
		Logger::Log("Error opening file.\n");
		return;
	}

	// WAVファイルのデータ部分にシーク
	audioFile.seekg(sizeof(WAVHeader));

	// ストリーミング用のバッファを複数作成
	constexpr int BUFFER_COUNT = 3; // バッファ数
	size_t BUFFER_SIZE = header.sampleRate * waveFormat.nBlockAlign; // バッファサイズ
	audioBuffers.resize(BUFFER_COUNT, std::vector<BYTE>(BUFFER_SIZE));
	XAUDIO2_BUFFER xAudioBuffers[BUFFER_COUNT] = {};
	StreamingVoiceCallback callback;


	// ソースボイスを作成し、コールバックを渡す
	if (FAILED(xAudio2_->CreateSourceVoice(&streamVoice, &waveFormat, XAUDIO2_VOICE_USEFILTER, XAUDIO2_MAX_FREQ_RATIO, &callback, nullptr))) {
		Logger::Log("Failed to create source voice.\n");
		return;
	}

	InitEffectChain();


	// ソースボイスを開始
	streamVoice->Start(0);
	// バッファリング処理
	int currentBufferIndex = 0;

	while (isStreaming.load()) {
		std::vector<BYTE>& currentBuffer = audioBuffers[currentBufferIndex];
		if (!ReadAudioData(audioFile, currentBuffer)) {
			// EOF
			if (isLoopStreaming.load()) {
				// EOF に達した場合、ファイルを先頭に戻してループ
				audioFile.clear();  // EOF flag をクリア
				audioFile.seekg(sizeof(WAVHeader), std::ios::beg);  // ヘッダーをスキップして再読み込み
				if (!ReadAudioData(audioFile, currentBuffer)) {
					break; // それでも読み込み失敗の場合はループ終了
				}
			}
			else if (!isLoopStreaming.load()) {
				break; // EOF
			}
			else {
				Logger::Log("Failed to ReadAudioData.\n");
				break; // 読み込みエラー
			}
		}

		// 現在のバッファを設定
		XAUDIO2_BUFFER& xBuffer = xAudioBuffers[currentBufferIndex];
		xBuffer.AudioBytes = static_cast<UINT32>(currentBuffer.size());
		xBuffer.pAudioData = currentBuffer.data();
		xBuffer.Flags = 0;

		// 最後のデータにはフラグを追加
		if (audioFile.eof()) {
			xBuffer.Flags = XAUDIO2_END_OF_STREAM;
		}

		// ソースボイスにバッファを送信
		if (FAILED(streamVoice->SubmitSourceBuffer(&xBuffer))) {
			Logger::Log("Failed to submit buffer.\n");
			break;
		}

		Logger::Log("Buffer submitted.\n");

		// 次のバッファを使用
		currentBufferIndex = (currentBufferIndex + 1) % BUFFER_COUNT;

		// コールバックで次のバッファの処理完了を待機
		callback.WaitForBuffer();
	}

	// クリーンアップ
	audioFile.close();
	streamVoice->Stop(0);
	streamVoice->DestroyVoice();
	streamVoice = nullptr;
	Logger::Log("Streaming finished.\n");
}

void StreamingAudio::InitEffectChain()
{
	// リバーブエフェクトを作成
	IUnknown* reverbEffect = nullptr;
	if (FAILED(XAudio2CreateReverb(&reverbEffect))) // Reverbエフェクトを作成
	{
		Logger::Log("Failed to create reverb effect.");
	}
	else {
		Logger::Log("succece to create reverb effect.");
	}
	// エフェクトチェーンの設定
	effect[0].pEffect = reverbEffect;  // リバーブエフェクトのインターフェース
	effect[0].InitialState = FALSE;		// 初期状態で無効化
	effect[0].OutputChannels = 2;      // ステレオ出力
	effectChain.EffectCount = 1;
	effectChain.pEffectDescriptors = effect;
	ApplyEffectChain();
	reverbEffect->Release();
}

bool StreamingAudio::ReadAudioData(std::ifstream& file, std::vector<BYTE>& buffer)
{
	if (!file.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
		return false; // EOF またはエラー
	}
	return true;
}

bool StreamingAudio::ReadWavHeader(std::string filename, WAVHeader& header)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file) return false;

	file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
	return (header.riff[0] == 'R' && header.riff[1] == 'I' && header.riff[2] == 'F' && header.riff[3] == 'F') &&
		(header.wave[0] == 'W' && header.wave[1] == 'A' && header.wave[2] == 'V' && header.wave[3] == 'E') &&
		(header.fmt[0] == 'f' && header.fmt[1] == 'm' && header.fmt[2] == 't' && header.fmt[3] == ' ');
}
