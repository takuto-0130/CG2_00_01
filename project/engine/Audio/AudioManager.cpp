#include "AudioManager.h"

void AudioManager::Initialize(const std::string& directoryPath)
{
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

void AudioManager::LoadWave(const char* filename)
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

	soundDataMap_[filename] = soundData;
}

void AudioManager::UnloadWave(const char* filename)
{
	auto it = soundDataMap_.find(filename);
	if (it != soundDataMap_.end()) {
		delete[] it->second.pBuffer;
		soundDataMap_.erase(it);
	}
}

AudioID AudioManager::PlayWave(const std::string& filename, const AudioType& audioType, bool isLoop)
{
	AudioID id;
	id.audioType = audioType;
	id.resourceNum = audio_[audioType]->PlayWave(filename.c_str(), isLoop);
	return id;
}




void AudioManager::StartStreaming(const std::string& filename, bool isLoop)
{
	// 既に存在する場合は再生を開始
	if (streamingAudioMap_.find(filename) != streamingAudioMap_.end()) {
		streamingAudioMap_[filename]->StartStreaming(filename, isLoop);
		return;
	}

	// 3つ以上ある場合は、最も古いストリームを削除
	if (streamingAudioMap_.size() >= maxStreamingInstances_) {
		RemoveOldestStreaming();
	}

	// 新しい StreamingAudio インスタンスを作成
	auto streamingAudio = std::make_unique<StreamingAudio>(xAudio2_.Get(), directoryPath_);
	streamingAudio->StartStreaming(filename, isLoop);

	// 管理リストに追加
	streamingAudioMap_[filename] = std::move(streamingAudio);
	streamingQueue_.push(filename);
}

void AudioManager::StopStreaming(const std::string& filename)
{
	if (streamingAudioMap_.find(filename) != streamingAudioMap_.end()) {
		streamingAudioMap_[filename]->StopStreaming();
		streamingAudioMap_.erase(filename);

		// `streamingQueue_` からも削除
		std::queue<std::string> tempQueue;
		while (!streamingQueue_.empty()) {
			if (streamingQueue_.front() != filename) {
				tempQueue.push(streamingQueue_.front());
			}
			streamingQueue_.pop();
		}
		streamingQueue_ = std::move(tempQueue);
	}
}

void AudioManager::RemoveOldestStreaming()
{
	if (streamingQueue_.empty()) return;

	// 最も古いエントリを取得
	std::string oldestFilename = streamingQueue_.front();
	streamingQueue_.pop();

	// 対応する `StreamingAudio` を削除
	if (streamingAudioMap_.find(oldestFilename) != streamingAudioMap_.end()) {
		streamingAudioMap_[oldestFilename]->StopStreaming();
		streamingAudioMap_.erase(oldestFilename);
	}
}

AudioManager::~AudioManager()
{
	for (auto& audio : audio_) {
		audio.reset(); // 明示的に解放
	}
	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
	for (auto& pair : streamingAudioMap_) {
		pair.second->StopStreaming();
	}
	streamingAudioMap_.clear();
	xAudio2_.Reset();
}
