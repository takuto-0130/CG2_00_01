#include "mathFunc.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <operatorOverload.h>
#include <algorithm>

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) { 
	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0], 
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1], 
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};
	return result;
}

float Length(const Vector3& v) { 
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

float Length(const Vector2& v) { 
	return sqrtf(v.x * v.x + v.y * v.y); 
}

Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	Vector3 result{v.x / len, v.y / len, v.z / len};
	return result;
}

float Dot(const Vector3& v1, const Vector3& v2) { 
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

Vector3 Subtruct(const Vector3& v1, const Vector3& v2) { 
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

Vector3 Add(const Vector3& v1, const Vector3& v2) { 
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; 
}

const Vector3 Multiply(const Vector3& v1, const Vector3& v2) {
	return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; 
}

const Vector3 Multiply(const Vector3& v, const float& s) { 
	return {v.x * s, v.y * s, v.z * s}; 
}

const Vector3 Multiply(const float& s, const Vector3& v) { 
	return {v.x * s, v.y * s, v.z * s}; 
}

float Lerp(const float& a, const float& b, float t) { 
	return a + t * (b - a);
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { 
	return v1 + t * (v2 - v1);
}

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) { 
	Vector3 nV1 = Normalize(v1);
	Vector3 nV2 = Normalize(v2);
	float dot = Dot(nV1, nV2);

	dot = (std::min)(dot, 1.0f);
	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);
	float sinThetaFrom = std::sin((1 - t) * theta);
	float sinThetaTo = std::sin(t * theta);
	Vector3 nSlerp{};
	if (sinTheta < 1.0e-5) {
		nSlerp = nV1;
	} else {
		nSlerp = (sinThetaFrom * nV1 + sinThetaTo * nV2) / sinTheta;
	}
	float length = Lerp(Length(v1), Length(v2), t);

	return length * nSlerp;
}

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) { 
	float s = 0.5f;		// 除算が重いので1/2の代用
	float t2 = t * t;	// tの2乗
	float t3 = t2 * t;	// tの3乗

	Vector3 e3 = -p0 + 3 * p1 - 3 * p2 + p3;
	Vector3 e2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
	Vector3 e1 = -p0 + p2;
	Vector3 e0 = 2 * p1;

	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
}

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) { 
	assert(points.size() >= 4 && "制御点は4点以上必要です");
	size_t division = points.size() - 1;
	float areaWidth = 1.0f / division;
	float t_2 = std::fmod(t, areaWidth) * division;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	size_t index = static_cast<size_t>(t / areaWidth);
	index = (std::min)(index, division - 1l);

	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;
	if (index == 0) {
		index0 = index1;
	}
	if (index3 >= points.size()) {
		index3 = index2;
	}
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];
	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) { 
	return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}
