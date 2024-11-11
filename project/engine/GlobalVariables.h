#pragma once
#include "struct.h"
#include <map>
#include <string>
#include <variant>
#include "json.hpp"

class GlobalVariables {
public:
	static GlobalVariables* GetInstance();

	void CreateGroup(const std::string& groupName);


	void SetValue(const std::string& groupName, const std::string& key, int32_t value);

	void AddItem(const std::string& groupName, const std::string& key, int32_t value);

	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;


	void SetValue(const std::string& groupName, const std::string& key, float value);

	void AddItem(const std::string& groupName, const std::string& key, float value);

	float GetFloatValue(const std::string& groupName, const std::string& key) const;


	void SetValue(const std::string& groupName, const std::string& key, const Vector3 value);

	void AddItem(const std::string& groupName, const std::string& key, const Vector3 value);

	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;



	void Update();

	/// <summary>
	/// ファイルに書き出し
	/// </summary>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// ファイルから読み込み
	/// </summary>
	void LoadFile(const std::string& groupName);

	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();

private:
	GlobalVariables();
	~GlobalVariables();
	GlobalVariables(const GlobalVariables& obj) = delete;
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;

private:
	struct Item {
		std::variant<int32_t, float, Vector3> value;
	};

	struct Group {
		std::map<std::string, Item> items;
	};

	std::map<std::string, Group> datas_;

	using json = nlohmann::json;
	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
};
