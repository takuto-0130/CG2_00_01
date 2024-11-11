#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>

struct Vector2 {
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix4x4 {
	std::array<std::array<float,4>, 4> m;
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texCoord;
	Vector3 normal;
};

//struct Material {
//	Vector4 color;
//	int32_t enableLighting;
//	float padding[3];
//	Matrix4x4 uvTransform;
//};
//
//struct TransfomationMatrix {
//	Matrix4x4 WVP;
//	Matrix4x4 World;
//};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct MateerialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MateerialData material;
};