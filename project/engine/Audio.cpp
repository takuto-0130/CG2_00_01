#include "Audio.h"
#include <cassert>
#include <algorithm>
#include <queue>

// ファイルからデータを読み込む関数
bool ReadAudioData(std::ifstream& file, std::vector<BYTE>& buffer) {
	if (!file.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
		return false; // EOF またはエラー
	}
	return true;
}

// WAVファイルのヘッダーを読み込む
bool ReadWavHeader(std::string filename, WAVHeader& header) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) return false;

	file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
	return (header.riff[0] == 'R' && header.riff[1] == 'I' && header.riff[2] == 'F' && header.riff[3] == 'F') &&
		(header.wave[0] == 'W' && header.wave[1] == 'A' && header.wave[2] == 'V' && header.wave[3] == 'E') &&
		(header.fmt[0] == 'f' && header.fmt[1] == 'm' && header.fmt[2] == 't' && header.fmt[3] == ' ');  // 'fmt 'チャンク
}

Audio::~Audio()
{
	StopStreaming();
	// BGMリソースの解放
	for (auto SourceVoice : pSourceVoices_)
	{
		if (SourceVoice != nullptr)
		{
			SourceVoice->DestroyVoice();
			SourceVoice = nullptr;
		}
	}
	if (streamVoice != nullptr) {
		streamVoice->DestroyVoice();
		streamVoice = nullptr;
	}
	if (masterVoice != nullptr)
	{
		masterVoice->DestroyVoice();
		masterVoice = nullptr;
	}
	xAudio2.Reset();
}

void Audio::StreamAudio(const char* filename) {

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
	if (FAILED(xAudio2->CreateSourceVoice(&streamVoice, &waveFormat, XAUDIO2_VOICE_USEFILTER, XAUDIO2_MAX_FREQ_RATIO, &callback, nullptr))) {
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
			if(isLoopStreaming.load()) { 
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

void Audio::Initialize(const std::string& directoryPath)
{
	directoryPath_ = directoryPath;
	HRESULT result;
	// インスタンスの生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイスの生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);

}
void Audio::StopBGM(int resourceNum)
{
	pSourceVoices_[resourceNum]->Stop();
	pSourceVoices_[resourceNum]->FlushSourceBuffers();
}

void Audio::PauseBGM(int resourceNum)
{
	pSourceVoices_[resourceNum]->Stop();
}

void Audio::ReStartBGM(int resourceNum)
{
	pSourceVoices_[resourceNum]->Start();
}

void Audio::SetBGMVolume(int resourceNum, float volume)
{
	pSourceVoices_[resourceNum]->SetVolume(/*std::clamp(*/volume/*, 0.0f, 1.0f)*/);
}

void Audio::LoadWave(const char* filename)
{
	//HRESULT result;
	if (soundDataMap.count(filename)) {
		// キーが存在する場合、処理を中断
		return;
	}

	// ファイル入力streamのインスタンス
	std::ifstream file;
	std::string filePath = directoryPath_;
	filePath += filename;
	filePath += ".wav";
	file.open(filePath, std::ios_base::binary);
	assert(file.is_open());

	// .wavデータ読み込み
	// RIFFヘッダーの読み込み
	RiffHeader riff;
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
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
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
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.playSoundLength = data.size / format.fmt.nBlockAlign;

	soundDataMap[filename] = soundData;
}

void Audio::SoundUnload(const char* filename)
{
	SoundData* soundData = &soundDataMap[filename];
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

int Audio::PlayWave(const char* filename, const bool isLoop)
{
	HRESULT result;

	SoundData& soundData = soundDataMap[filename];

	// 今回使うサウンドデータ
	int sourceNum = -1;

	// 使用できるリソースを検索
	sourceNum = SearchSourceVoice(pSourceVoices_.data());

	// 使用できるリソースがない場合は-1を返す
	if (sourceNum == -1) {
		Logger::Log("No sound resource available.\n"); 
		return -1; 
	}

	// 再生停止中、もしくは残りの再生数が最小のリソースを使用
	if (pSourceVoices_[sourceNum] != nullptr)
	{
		pSourceVoices_[sourceNum]->Stop();
		pSourceVoices_[sourceNum]->FlushSourceBuffers();
	}

	// 波形フォーマットをもとにSourceVoiceの生成
	if (FAILED(xAudio2->CreateSourceVoice(&pSourceVoices_[sourceNum], &soundData.wfex))) {
		Logger::Log("Failed to create source voice.\n");
		return -1;
	}

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = SetBuffer(isLoop, soundData);

	// 波形データの再生
	result = pSourceVoices_[sourceNum]->SubmitSourceBuffer(&buf);
	result = pSourceVoices_[sourceNum]->Start();

	return sourceNum;
}

int Audio::SearchSourceVoice(IXAudio2SourceVoice** sourceVoices)
{
	// 今回再生するリソース
	int sourceVoiceNum = -1;

	// リソースのバッファ
	unsigned int soundBuffer = 0;

	// 使用できる再生リソースを検索
	for (int i = 0; i < kMaxPlayWave; i++)
	{
		if (sourceVoices[i] == nullptr) {
			sourceVoiceNum = i;
			break;
		}
		// 現在の状態を取得
		XAUDIO2_VOICE_STATE state;
		sourceVoices[i]->GetState(&state);

		// バッファが0ならば再生可能と判断
		if (state.BuffersQueued == 0)
		{
			sourceVoiceNum = i;
			break;
		}
		else
		{
			// 初期値もしくはバッファが最小の場合は入れ替え
			if (soundBuffer == 0 || soundBuffer > state.BuffersQueued)
			{
				soundBuffer = state.BuffersQueued;
				sourceVoiceNum = i;
			}
		}
	}

	return sourceVoiceNum;
}

XAUDIO2_BUFFER Audio::SetBuffer(bool loop, const SoundData& sound)
{
	// バッファ設定
	XAUDIO2_BUFFER buffer;

	// バッファの初期化
	memset(&buffer, 0x00, sizeof(buffer));
	buffer.pAudioData = sound.pBuffer;
	buffer.AudioBytes = sound.bufferSize;
	buffer.PlayBegin = 0;
	buffer.PlayLength = sound.playSoundLength;

	// ループ設定
	if (loop)
	{
		buffer.LoopBegin = 0;
		buffer.LoopLength = sound.playSoundLength;
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	return buffer;
}
