#pragma once
#include "struct.h"
#include <cmath>
#include <assert.h>

//行列の加法
Matrix4x4 Add(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

//行列の減法
Matrix4x4 Subtract(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

//行列の積
Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

//転置行列
Matrix4x4 Transpose(const Matrix4x4& matrix);

//単位行列
Matrix4x4 MakeIdentity4x4();


//X軸
Matrix4x4 MakeRotateXMatrix(float radian);

//Y軸
Matrix4x4 MakeRotateYMatrix(float radian);

//Z軸
Matrix4x4 MakeRotateZMatrix(float radian);

//アフィン変換3D
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

//ビューポート変換
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);




Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Vector3 TransformM(const Vector3& vector, const Matrix4x4& matrix);


