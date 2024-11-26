#pragma once
#include "struct.h"
#include "Matrix4x4Func.h"

Vector3 operator+(const Vector3& a, const Vector3& b);

Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a);

Vector3 operator*(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& v, const float& s);
Vector3 operator*(const float& s, const Vector3& v);

Vector3 operator/(const Vector3& a, const Vector3& b);
Vector3 operator/(const Vector3& v, const float& s);

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);


Vector2 operator-(const Vector2& a, const Vector2& b);