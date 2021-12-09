#include "GameScene.h"
#include "Collision.h"
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace DirectX;

const int map_size = 3;
const int map_max_x = 20, map_max_y = 10;

//CSVファイル読み込み
void CsvToVector(std::vector<std::vector<int>>& mapData, std::string fName) {
	std::ifstream ifs(fName);
	std::string line;
	std::vector<int> csv;
	while (getline(ifs, line))
	{
		std::istringstream stream(line);
		std::string field;
		std::vector<int> result;
		while (std::getline(stream, field, ','))
		{
			result.push_back(stoi(field));
		}
		for (auto i : result)
		{
			csv.push_back(i);
		}
	}
	mapData.push_back(csv);
}

//マップナンバー指定
int GetChipNum(int x, int y, std::vector<int> map)
{
	const int X = x / map_size;
	const int Y = y / map_size;

	if (X < 0 || X >= map_max_x || Y < 0 || Y >= map_max_y)
	{
		return 0;
	}

	return map[Y * map_max_x + X];
}

GameScene::GameScene()
{

}

GameScene::~GameScene()
{
	safe_delete(sprite);
	for (int i = 0; i < _countof(object3d); i++)
	{
		safe_delete(object3d[i]);
	}
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
	if (!Sprite::LoadTexture(1, L"Resources/frame.png"))
	{
		assert(0);
	}

	//マップ読み込み
	std::vector<std::vector<int>> map;
	CsvToVector(map, "Resource/Draw/なんたら.csv");//mapNum=0

	// 背景スプライト生成
	sprite = Sprite::Create(1, { 0.0f,0.0f });
	sprite->SetSize({ 1280.0f,720.0f });
	sprite->SetPosition({ 0.0f,0.0f });

	//.objの名前を指定してモデルを読み込む
	modelFighter = Model::CreateFromObject("sphere", true);
	modelCube = Model::CreateFromObject("cube", false);
	//scaleの初期値の代入
	for (int i = 0; i < 50; i++)
	{
		objectSize[i] = { 3,3,3 };
	}

	for (int i = 0; i < _countof(object3d); i++)
	{
		// 3Dオブジェクト生成
		object3d[i] = Object3d::Create();
		// 3Dオブジェクトにモデルを割り当てる
		if (i == 0)
		{
			object3d[i]->SetModel(modelFighter);
		}
		else
		{
			object3d[i]->SetModel(modelCube);
		}
		object3d[i]->SetScale({ 3, 3, 3 });
		object3d[i]->SetColor({ 1, 1, 1 ,1 });
		if (i < 15)
		{
			object3d[i]->SetPosition({ -10 + ((float)i * 3),2,0 });
		}
		else if (i < 20)
		{
			object3d[i]->SetPosition({ -10 + ((float)i * 3),10,0 });
		}
		else if (i < 30)
		{
			object3d[i]->SetPosition({ 10 + ((float)i * 3),2,0 });
		}
	}

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
	//audio->PlayWave("Resources/Alarm01.wav");

	// カメラ注視点をセット
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,10,-50 });
}

void GameScene::Update()
{
	//マウス座標
	POINT mousePos;
	GetCursorPos(&mousePos);
	if (phase == Move)
	{
		//playerの処理ここから下
		if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
		{
			// 現在の座標を取得
			XMFLOAT3 playerPos = object3d[0]->GetPosition();
			XMFLOAT3 blockPos[9] = {};
			for (int i = 0; i < 9; i++)
			{
				blockPos[i] = object3d[i + 1]->GetPosition();
			}

			// 移動後の座標を計算
			if (input->PushKey(DIK_W))
			{
				playerPos.y += 1.0f;
			}
			else if (input->PushKey(DIK_S))
			{
				playerPos.y -= 1.0f;
			}

			if (input->PushKey(DIK_D))
			{
				playerPos.x += 1.0f;
			}
			else if (input->PushKey(DIK_A))
			{
				playerPos.x -= 1.0f;
			}
			// 座標の変更を反映
			object3d[0]->SetPosition(playerPos);
		}
		if (input->PushKey(DIK_RETURN))
		{
			phase = Trimming;
		}
	}
	//ステージの処理ここから下
	if (phase == Trimming)
	{
		if (process == None)
		{
			if (input->PushKey(DIK_1))
			{
				process = Size;
			}
			if (input->PushKey(DIK_2))
			{
				process = Cut;
			}
			if (input->PushKey(DIK_3))
			{
				process = Rota;
			}
			if (input->PushKey(DIK_4))
			{
				process = Color;
			}
		}
		//process1 床のサイズ変更
		if (process == Size)
		{
			//色の変更
			object3d[selectBlock]->SetColor({ 1, 0, 1 ,1 });
			//オブジェクトのサイズ変更
			if (input->PushKey(DIK_UP))
			{
				objectSize[selectBlock].x += 1;
				objectSize[selectBlock].y += 1;
			}
			if (input->PushKey(DIK_DOWN))
			{
				objectSize[selectBlock].x -= 1;
				objectSize[selectBlock].y -= 1;
			}
			//サイズ変更をするオブジェクト変更
			if (input->PushKey(DIK_LEFT) && selectBlock > 1)
			{
				selectBlock -= 1;
			}
			if (input->PushKey(DIK_RIGHT))
			{
				selectBlock += 1;
			}
			object3d[selectBlock]->SetScale({ objectSize[selectBlock] });
		}
	}

	//カメラの処理ここから下

	cameraPos = object3d[0]->GetPosition();

	camera->SetEye({ cameraPos.x, cameraPos.y , -50 });
	camera->SetTarget({ cameraPos.x , cameraPos.y , 0 });
	//アップデートここから下
	for (int i = 0; i < _countof(object3d); i++)
	{
		object3d[i]->Update();
	}
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
	if (phase == Trimming)
	{
		sprite->Draw();
	}
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion 背景スプライト描画
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3Dオブクジェクトの描画
	for (int i = 0; i < _countof(object3d); i++)
	{
		object3d[i]->Draw();
	}
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