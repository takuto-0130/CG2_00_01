#include "operatorOverload.h"


Vector3 operator+(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}


Vector3 operator-(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

Vector3 operator-(const Vector3& a) {
	Vector3 result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	return result;
}


Vector3 operator*(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;
	return result;
}
Vector3 operator*(const Vector3& v, const float& s) { 
	Vector3 result;
	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;
	return result;
}
Vector3 operator*(const float& s, const Vector3& v) {
	Vector3 result;
	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;
	return result;
}


Vector3 operator/(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x / b.x;
	result.y = a.y / b.y;
	result.z = a.z / b.z;
	return result;
}
Vector3 operator/(const Vector3& v, const float& s) {
	Vector3 result;
	result.x = v.x / s;
	result.y = v.y / s;
	result.z = v.z / s;
	return result;
}

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { 
	return Multiply(m1, m2); }

Vector2 operator-(const Vector2& a, const Vector2& b) {
	Vector2 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}
