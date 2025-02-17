#include "Audio.h"
#include <cassert>
#include <algorithm>
#include <queue>

Audio::~Audio()
{
	// BGMリソースの解放
	for (auto SourceVoice : pSourceVoices_)
	{
		if (SourceVoice)
		{
			SourceVoice->DestroyVoice();
			SourceVoice = nullptr;
		}
	}
	if (submixVoice_) {
		submixVoice_->DestroyVoice();
		submixVoice_ = nullptr;
	}
}

void Audio::Initialize(Microsoft::WRL::ComPtr<IXAudio2> xAudio2, const std::string& directoryPath)
{
	directoryPath_ = directoryPath;
	HRESULT result;

	xAudio2_ = xAudio2;

	// サブミックスボイスの生成
	result = xAudio2_->CreateSubmixVoice(&submixVoice_, 2, 44100);
	if (FAILED(result)) {
		Logger::Log("Failed to create submix voice");
		return;
	}

}
void Audio::StopAudio(int resourceNum)
{
	pSourceVoices_[resourceNum]->Stop();
	pSourceVoices_[resourceNum]->FlushSourceBuffers();
}

void Audio::PauseAudio(int resourceNum)
{
	pSourceVoices_[resourceNum]->Stop();
}

void Audio::ReStartAudio(int resourceNum)
{
	pSourceVoices_[resourceNum]->Start();
}

void Audio::SetAudioVolume(int resourceNum, float volume)
{
	pSourceVoices_[resourceNum]->SetVolume(/*std::clamp(*/volume/*, 0.0f, 1.0f)*/);
}

int Audio::PlayWave(const char* filename, const bool isLoop)
{
	HRESULT result;

	SoundData& soundData = soundDataMap_[filename];

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

	XAUDIO2_SEND_DESCRIPTOR sendDesc = { 0, submixVoice_ };
	XAUDIO2_VOICE_SENDS sendList = { 1, &sendDesc };

	// 波形フォーマットをもとにSourceVoiceの生成
	if (FAILED(xAudio2_->CreateSourceVoice(&pSourceVoices_[sourceNum], &soundData.wfex, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &sendList))) {
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
