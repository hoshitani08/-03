#pragma once

#include <DirectXMath.h>

struct Sphere //球
{
	//中心座標
	DirectX::XMVECTOR center = { 0,0,0,1 };
	//半径
	float radius = 1.0f;
};

struct Plane //平面
{
	//法線ベクトル
	DirectX::XMVECTOR normal = { 0,1,0,0 };
	//原点(0,0,0)からの距離
	float distance = 0.0f;
};

class Triangle //法線付きの三角形
{
public:
	//頂点座標3つ
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
	//法線ベクトル
	DirectX::XMVECTOR normal;

public:
	//法線の計算
	void ComputeNormal();
};

struct Ray //レイ(半直線)
{
	//始点座標
	DirectX::XMVECTOR start = { 0, 0, 0, 1 };
	//方向
	DirectX::XMVECTOR dir = { 1, 0, 0, 0 };
};