#include "GlobalVariables.h"
#ifdef _DEBUG

#include "imgui.h"

#endif // _DEBUG
#include "fstream"
#include "Windows.h"
GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;
	return &instance;
}

void GlobalVariables::CreateGroup(const std::string& groupName) {
	datas_[groupName];
}


void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int32_t value) {
	Group& group = datas_[groupName];
	Item newItem{};
	newItem.value = value;
	group.items[key] = newItem;
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value) {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());

	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.items.find(key);
	if (itItem == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key) const {
	std::map<std::string, Group>::const_iterator itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());
	const Group& group = datas_.at(groupName);
	std::map<std::string, Item>::const_iterator itItem = group.items.find(key);
	assert(itItem != group.items.end());
	if (std::holds_alternative<int32_t>(group.items.at(key).value)) {
		int32_t value = std::get<int32_t>(group.items.at(key).value);
		return value;
	}
	return 0;
}


void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	Group& group = datas_[groupName];
	Item newItem{};
	newItem.value = value;
	group.items[key] = newItem;
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, float value) {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());

	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.items.find(key);
	if (itItem == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key) const {
	std::map<std::string, Group>::const_iterator itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());
	const Group& group = datas_.at(groupName);
	std::map<std::string, Item>::const_iterator itItem = group.items.find(key);
	assert(itItem != group.items.end());
	if (std::holds_alternative<float>(group.items.at(key).value)) {
		float value = std::get<float>(group.items.at(key).value);
		return value;
	}
	return 0;
}


void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector3 value) {
	Group& group = datas_[groupName];
	Item newItem{};
	newItem.value = value;
	group.items[key] = newItem;
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector3 value) {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());

	Group& group = datas_[groupName];
	std::map<std::string, Item>::iterator itItem = group.items.find(key);
	if (itItem == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key) const {
	std::map<std::string, Group>::const_iterator itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());
	const Group& group = datas_.at(groupName);
	std::map<std::string, Item>::const_iterator itItem = group.items.find(key);
	assert(itItem != group.items.end());
	if (std::holds_alternative<Vector3>(group.items.at(key).value)) {
		Vector3 value = std::get<Vector3>(group.items.at(key).value);
		return value;
	}
	return Vector3(0, 0, 0);
}


void GlobalVariables::Update() {

#ifdef _DEBUG

	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end(); ++itGroup) {
		const std::string& groupName = itGroup->first;
		Group& group = itGroup->second;
		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}

		// 各項目について
		for (std::map<std::string, Item>::iterator itItem = group.items.begin(); itItem != group.items.end(); ++itItem) {
			const std::string& itemName = itItem->first;
			Item& item = itItem->second;

			if (std::holds_alternative<int32_t>(item.value)) {
				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			else if (std::holds_alternative<float>(item.value)) {
				float* ptr = std::get_if<float>(&item.value);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0, 100);
			}
			else if (std::holds_alternative<Vector3>(item.value)) {
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), -10.0f, 10.0f);
			}
		}

		// 改行
		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}



	ImGui::EndMenuBar();
	ImGui::End();

#endif // _DEBUG

}

void GlobalVariables::SaveFile(const std::string& groupName) {
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);
	// 未登録チェック
	assert(itGroup != datas_.end());

	json root;
	root = json::object();

	//jsonオブジェクト登録
	root[groupName] = json::object();

	// 各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin(); itItem != itGroup->second.items.end(); ++itItem) {
		// 項目を取得
		const std::string& itemName = itItem->first;
		// 項目の参照を取得
		Item& item = itItem->second;

		if (std::holds_alternative<int32_t>(item.value)) {
			root[groupName][itemName] = std::get<int32_t>(item.value);
		}
		else if (std::holds_alternative<float>(item.value)) {
			root[groupName][itemName] = std::get<float>(item.value);
		}
		else if (std::holds_alternative<Vector3>(item.value)) {
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = json::array({ value.x, value.y, value.z });
		}
	}
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}
	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込みように開く
	ofs.open(filePath);
	// ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	// 閉じる
	ofs.close();
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	// 読み込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	// 読み込み用ファイルストリーム 
	std::ifstream ifs;
	// ファイルを読み込み用に開く
	ifs.open(filePath);
	if (!ifs.is_open()) {
		MessageBoxA(nullptr, "error", "GlobalVariables", 0);
		assert(true);
	}
	json root;

	ifs >> root;
	ifs.close();
	// グループを探索
	json::iterator itGroup = root.find(groupName);

	// 未登録チェック
	assert(itGroup != root.end());

	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		// アイテム名を取得
		const std::string& itemName = itItem.key();
		if (itItem->is_number_integer()) {
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_number_float()) {
			float value = itItem->get<float>();
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_array() && itItem->size() == 3) {
			// float型のjson配列登録
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);
		}
	}
}

void GlobalVariables::LoadFiles() {
	std::filesystem::path dir(kDirectoryPath);
	// ディレクトリがなければスキップする
	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}
	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& enty : dir_it) {
		// ファイルパスを取得
		const std::filesystem::path& filePath = enty.path();

		// ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		// .jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}
		LoadFile(filePath.stem().string());
	}
}


GlobalVariables::GlobalVariables() {}

GlobalVariables::~GlobalVariables() {}
