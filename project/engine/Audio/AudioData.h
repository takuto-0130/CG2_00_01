#pragma once

/**
 * @enum AudioType
 * @brief オーディオの種類を表す列挙型
 */
enum AudioType {
    kBGM,						///< BGM（背景音楽）
    kGameSE,					///< ゲーム内のSE（攻撃音など）
    kUISE,						///< UIのSE（ボタン音など）
    kAmbientSound,				///< 環境音（川の流れなど）
    kTypeNum					///< オーディオタイプの総数（新しい種類を追加する場合はこの前に定義）
};

/**
 * @brief オーディオの識別情報
 */
struct AudioID {
    int resourceNum;			///< リソース番号
    AudioType audioType;		///< 音声の種類
};

/**
 * @brief 音声データ
 */
struct SoundData {
	WAVEFORMATEX wfex;			///< 波形フォーマット
	BYTE* pBuffer;				///< バッファの先頭アドレス
	unsigned int bufferSize;	///< バッファのサイズ
	int playSoundLength;
};

/**
 * @brief チャンクヘッダー
 */
struct ChunkHeader {
	char id[4];					///< チャンクごとのID
	int32_t size;				///< チャンクサイズ
};

/**
 * @brief RIFFヘッダチャンク
 */
struct RiffHeader {
	ChunkHeader chunk;			///< "RIFF"
	char type[4];				///< "WAVE"
};

/**
 * @brief FMTチャンク
 */
struct FormatChunk {
	ChunkHeader chunk;			///< "fmt"
	WAVEFORMATEX fmt;			///< 波形フォーマット
};

////////////////////////////////////////////////////////////////////

/**
 * @brief WAVヘッダーの定義（ストリーミング再生用）
 */
struct WAVHeader {
	char riff[4];				///< "RIFF"
	uint32_t size;				///< ファイルサイズ
	char wave[4];				///< "WAVE"
	char fmt[4];				///< "fmt "
	uint32_t fmtSize;			///< fmtチャンクのサイズ
	uint16_t audioFormat;		///< オーディオフォーマット（1はPCM）
	uint16_t numChannels;		///< チャネル数
	uint32_t sampleRate;		///< サンプルレート
	uint32_t byteRate;			///< バイトレート
	uint16_t blockAlign;		///< ブロックアライメント
	uint16_t bitsPerSample;		///< サンプルあたりのビット数
	char data[4];				///< "data"
	uint32_t dataSize;			///< データチャンクのサイズ
};
