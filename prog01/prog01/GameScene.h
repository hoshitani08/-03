#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"
#include "Audio.h"
#include "Model.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "CollisionPrimitive.h"
#include "Camera.h"
#include "Light.h"
#include "Ease.h"

class GameScene
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数
	// コンストクラタ
	GameScene();
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);
	// 毎フレーム処理
	void Update();
	// 描画
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText debugText;
	struct Block
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Size;
		XMFLOAT3 Rotation;
	};
	Block block[50];
	XMFLOAT3 cameraPos = { 0,10,-50 };
	float cameraPosZ = -50.0f;
	const float cameraUpZ = -60.0f;
	const float cameraNormalZ = -50.0f;
	enum Phase
	{
		Move, Trimming
	};
	enum Process
	{
		None, Size, Cut, Rota, Color
	};
	bool phase = Move;
	bool phaseChange = false;
	int process = None;
	int selectBlock = 1;
	//イージングの進行度用
	float nowCount = 0.0f;
	float timeRate = 0.0f;
	// ゲームシーン用
	Sprite* sprite[6] = { nullptr };
	XMFLOAT2 SmartphoneSize = { 128,72 };
	const XMFLOAT2 SmartphoneMinSize = { 128,72 };
	const XMFLOAT2 SmartphoneMaxSize = { 1280,720 };
	//オブジェクト
	Model* modelFighter = nullptr;
	Model* modelCube = nullptr;
	Object3d* object3d[50] = { nullptr };
	//パーティクル
	ParticleManager* particleMan = nullptr;
	ParticleEmitter* particleEmitter = nullptr;
	//カメラ
	Camera* camera = nullptr;
	//ライト
	Light* light = nullptr;
	//当たり判定(球)
	Sphere sphere;
	//当たり判定(平面)
	Plane plane;
	//当たり判定(三角形)
	Triangle triangle;
	//当たり判定(レイ)
	Ray ray;
};