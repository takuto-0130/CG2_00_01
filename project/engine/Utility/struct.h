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

	Vector3& operator+=(const Vector3& a) {
		x = a.x + x;
		y = a.y + y;
		z = a.z + z;
		return *this;
	}

	Vector3& operator-=(const Vector3& a) {
		x = x - a.x;
		y = y - a.y;
		z = z - a.z;
		return *this;
	}

	Vector3& operator*=(const float& a) {
		x = x * a;
		y = y * a;
		z = z * a;
		return *this;
	}

	Vector3& operator/=(const float& a) {
		x = x / a;
		y = y / a;
		z = z / a;
		return *this;
	}

	bool operator==(const Vector3& a) const {
		return (x == a.x && y == a.y && z == a.z);
	}
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

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

//struct TransfomationMatrix {
//	Matrix4x4 WVP;
//	Matrix4x4 World;
//};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};