#include "GameScene.h"
#include "Collision.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

GameScene::GameScene()
{

}

GameScene::~GameScene()
{
	safe_delete(sprite);
	safe_delete(object3d);
	safe_delete(modelFighter);
	safe_delete(particleMan);
	safe_delete(particleEmitter);
	safe_delete(camera);
	safe_delete(light);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// カメラ生成
	camera = new Camera(WinApp::window_width, WinApp::window_height);

	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera);

	// デバッグテキスト用テクスチャ読み込み
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png"))
	{
		assert(0);
	}
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	if (!Sprite::LoadTexture(1, L"Resources/APEX_01.png"))
	{
		assert(0);
	}

	// 背景スプライト生成
	sprite = Sprite::Create(1, { 0.0f,0.0f });
	sprite->SetSize({ 100.0f,100.0f });
	sprite->SetPosition({ 100.0f,100.0f });

	//.objの名前を指定してモデルを読み込む
	modelFighter = Model::CreateFromObject("sphere", true);
	// 3Dオブジェクト生成
	object3d = Object3d::Create();
	// 3Dオブジェクトにモデルを割り当てる
	object3d->SetModel(modelFighter);
	object3d->SetScale({ 3, 3, 3 });
	object3d->SetPosition({ 0,2,0 });

	// パーティクルマネージャ生成
	particleMan = ParticleManager::Create(dxCommon->GetDevice(), camera);
	particleEmitter = ParticleEmitter::Create();
	particleEmitter->SetParticleManager(particleMan);

	//ライト生成
	light = Light::Create();
	//ライトの色を設定
	light->SetLightColor({ 1, 1, 1 });
	light->SetLightDir({ 0, 1, 5, 0 });
	//3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	//サウンド再生
	audio->PlayWave("Resources/Alarm01.wav");

	// カメラ注視点をセット
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,10,-15 });
}

void GameScene::Update()
{
	//キーが押されているときの処理
	if (input->TriggerPadRight())
	{
		OutputDebugStringA("Hit 0\n");
	}

	////X座標、Y座標を指定して表示
	//debugText.Print("Hello,DirectX!!", 200, 100, 1.0f);
	////X座標、Y座標、縮尺を指定して表示
	//debugText.Print("Nihon Kogakuin", 200, 200, 2.0f);

	// オブジェクト移動
	//if (input->PushPadStickUp() || input->PushPadStickDown() || input->PushPadStickRight() || input->PushPadStickLeft())
	//{
	//	// 現在の座標を取得
	//	XMFLOAT3 position = object3d->GetPosition();
	//
	//	// 移動後の座標を計算
	//	if (input->PushPadStickUp())
	//	{
	//		position.y += 1.0f;
	//	}
	//	else if (input->PushPadStickDown())
	//	{
	//		position.y -= 1.0f;
	//	}
	//
	//	if (input->PushPadStickRight())
	//	{
	//		position.x += 1.0f;
	//	}
	//	else if (input->PushPadStickLeft())
	//	{
	//		position.x -= 1.0f;
	//	}
	//
	//	// 座標の変更を反映
	//	object3d->SetPosition(position);
	//}

	XMFLOAT3 rot = object3d->GetRotation();
	rot.y += 1.0f;
	object3d->SetRotation(rot);

	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_W))
		{
			camera->CameraMoveEyeVector({ 0.0f,+1.0f,0.0f });
		}
		else if (input->PushKey(DIK_S))
		{
			camera->CameraMoveEyeVector({ 0.0f,-1.0f,0.0f });
		}
		
		if (input->PushKey(DIK_D))
		{
			camera->CameraMoveEyeVector({ +1.0f,0.0f,0.0f });
		}
		else if (input->PushKey(DIK_A))
		{
			camera->CameraMoveEyeVector({ -1.0f,0.0f,0.0f });
		}
	}

	if (input->PushPadStickUp())
	{
		debugText.Print("Up", 200, 100, 1.0f);
	}
	else if (input->PushPadStickDown())
	{
		debugText.Print("Down", 200, 100, 1.0f);
	}

	if (input->PushPadStickRight())
	{
		debugText.Print("Right", 240, 100, 1.0f);
	}
	else if (input->PushPadStickLeft())
	{
		debugText.Print("Left", 240, 100, 1.0f);
	}

	particleEmitter->Add();

	object3d->Update();
	particleEmitter->Update();
	camera->Update();
	light->Update();
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 背景スプライト描画
	sprite->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion 背景スプライト描画
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3Dオブクジェクトの描画
	object3d->Draw();
	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画
#pragma region パーティクル
	//particleEmitter->Draw(dxCommon->GetCommandList());
#pragma endregion パーティクル
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// デバッグテキストの描画
	debugText.DrawAll(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion 前景スプライト描画
}