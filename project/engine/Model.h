#pragma once
#include "ModelLoader.h"
#include "struct.h"
#include "Matrix4x4Func.h"
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Camera.h>

class WorldTransform;
class Model
{
public: // メンバ関数
	// 初期化
	void Initialize(ModelLoader* modelLoader, const std::string& directoryPath, const std::string& fileName);

	// 描画
	void Draw(WorldTransform& transform, Camera* camera);

	// 頂点データの生成
	void CreateVertexResource();
private: // 構造体
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Color {
		float r, g, b;
	};

	struct MaterialData {
		std::string name;
		float Ns;
		Color Ka; // 環境光色
		Color Kd; // 拡散反射色
		Color Ks; // 鏡面反射光
		float Ni;
		float d;
		uint32_t illum;
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	struct Node {
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};

	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
		Node rootNode;
	};

private:
	ModelLoader* modelLoader_ = nullptr;

	ModelData modelData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	VertexData* vertexData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

public:
	// objロード
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	// ノードをモデルデータに変換
	static Node ReadNode(aiNode* node);
public:
	void SetVertices(VertexData vertex);
	void SetTexturePath(const std::string& filePath) { modelData_.material.textureFilePath = filePath; }
};


