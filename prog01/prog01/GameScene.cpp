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

//CSV�t�@�C���ǂݍ���
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

//�}�b�v�i���o�[�w��
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
	for (int i = 0; i < _countof(sprite); i++)
	{
		safe_delete(sprite[i]);
	}
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
	// nullptr�`�F�b�N
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// �J��������
	camera = new Camera(WinApp::window_width, WinApp::window_height);

	// 3D�I�u�W�F�N�g�ɃJ�������Z�b�g
	Object3d::SetCamera(camera);

	// �f�o�b�O�e�L�X�g�p�e�N�X�`���ǂݍ���
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png"))
	{
		assert(0);
	}
	// �f�o�b�O�e�L�X�g������
	debugText.Initialize(debugTextTexNumber);

	// �e�N�X�`���ǂݍ���
	if (!Sprite::LoadTexture(1, L"Resources/frame.png"))
	{
		assert(0);
	}
	// �e�N�X�`���ǂݍ���
	if (!Sprite::LoadTexture(2, L"Resources/icon1.png"))
	{
		assert(0);
	}
	// �e�N�X�`���ǂݍ���
	if (!Sprite::LoadTexture(3, L"Resources/icon2.png"))
	{
		assert(0);
	}

	//�}�b�v�ǂݍ���
	std::vector<std::vector<int>> map;
	CsvToVector(map, "Resource/Draw/�Ȃ񂽂�.csv");//mapNum=0

	sprite[0] = Sprite::Create(1, { 0.0f,0.0f });
	sprite[1] = Sprite::Create(2, { 0.0f,0.0f });
	sprite[2] = Sprite::Create(3, { 0.0f,0.0f });
	sprite[0]->SetSize({ 1280.0f,720.0f });
	sprite[1]->SetSize({ 128.0f,128.0f });
	sprite[2]->SetSize({ 128.0f,128.0f });
	// �w�i�X�v���C�g����
	for (int i = 0; i < _countof(sprite); i++)
	{
		sprite[i]->SetPosition({ 0.0f,0.0f });
	}

	//.obj�̖��O���w�肵�ă��f����ǂݍ���
	modelFighter = Model::CreateFromObject("sphere", true);
	modelCube = Model::CreateFromObject("cube", false);
	//scale�̏����l�̑��
	for (int i = 0; i < 50; i++)
	{
		block[i].Size = { 3,3,3 };
	}

	for (int i = 0; i < _countof(object3d); i++)
	{
		// 3D�I�u�W�F�N�g����
		object3d[i] = Object3d::Create();
		// 3D�I�u�W�F�N�g�Ƀ��f�������蓖�Ă�
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
		object3d[i]->SetRotation({ 0,0,0 });
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
		block[i].Pos = object3d[i]->GetPosition();
	}

	// �p�[�e�B�N���}�l�[�W������
	particleMan = ParticleManager::Create(dxCommon->GetDevice(), camera);
	particleEmitter = ParticleEmitter::Create();
	particleEmitter->SetParticleManager(particleMan);

	//���C�g����
	light = Light::Create();
	//���C�g�̐F��ݒ�
	light->SetLightColor({ 1, 1, 1 });
	light->SetLightDir({ 0, 1, 5, 0 });
	//3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	//�T�E���h�Đ�
	//audio->PlayWave("Resources/Alarm01.wav");

	// �J���������_���Z�b�g
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,10,-50 });
}

void GameScene::Update()
{
	//�}�E�X���W
	POINT mousePos;
	GetCursorPos(&mousePos);
	if (phase == Move)
	{
		//player�̏����������牺
		if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
		{
			// ���݂̍��W���擾
			XMFLOAT3 playerPos = object3d[0]->GetPosition();
			XMFLOAT3 blockPos[9] = {};
			for (int i = 0; i < 9; i++)
			{
				blockPos[i] = object3d[i + 1]->GetPosition();
			}

			// �ړ���̍��W���v�Z
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
			// ���W�̕ύX�𔽉f
			object3d[0]->SetPosition(playerPos);
			cameraPos = object3d[0]->GetPosition();

			camera->SetEye({ cameraPos.x, cameraPos.y , -50 });
			camera->SetTarget({ cameraPos.x , cameraPos.y , 0 });
			camera->Update();
		}
		if (input->PushKey(DIK_RETURN))
		{
			phase = Trimming;
			camera->SetEye({ cameraPos.x, cameraPos.y , -60 });
			camera->Update();
		}
	}
	//�X�e�[�W�̏����������牺
	if (phase == Trimming)
	{
		if (process == None)
		{
			if (input->PushKey(DIK_1))
			{
				//�u���b�N�̃T�C�Y�ύX
				process = Size;
			}
			else if (input->PushKey(DIK_2))
			{
				//�R�s�y
				process = Cut;
			}
			else if (input->PushKey(DIK_3))
			{
				//�u���b�N�̉�]
				process = Rota;
			}
			else if (input->PushKey(DIK_4))
			{
				//�ʓx�A���x�̕ύX
				process = Color;
			}
			else if (input->PushKey(DIK_BACK))
			{
				phase = Move;
				camera->SetEye({ cameraPos.x, cameraPos.y , -50 });
				camera->Update();
			}
		}
		//process1 ���̃T�C�Y�ύX
		if (process == Size)
		{
			//�F�̕ύX
			object3d[selectBlock]->SetColor({ 1, 0, 1 ,1 });
			//�I�u�W�F�N�g�̃T�C�Y�ύX
			if (input->PushKey(DIK_UP))
			{
				block[selectBlock].Size.x += 1;
				block[selectBlock].Size.y += 1;
			}
			else if (input->PushKey(DIK_DOWN))
			{
				block[selectBlock].Size.x -= 1;
				block[selectBlock].Size.y -= 1;
			}
			//�T�C�Y�ύX������I�u�W�F�N�g�ύX
			else if (input->PushKey(DIK_LEFT) && selectBlock > 1)
			{
				selectBlock -= 1;
			}
			else if (input->PushKey(DIK_RIGHT))
			{
				selectBlock += 1;
			}
			else if (input->PushKey(DIK_BACK))
			{
				process = None;
			}
			object3d[selectBlock]->SetScale({ block[selectBlock].Size });
		}
		//�R�s�[�y�[�X�g
		if (process == Cut)
		{
			//�I�u�W�F�N�g
			if (input->PushKey(DIK_UP))
			{
				block[selectBlock].Pos.y += 1;
			}
			else if (input->PushKey(DIK_DOWN))
			{
				block[selectBlock].Pos.y -= 1;
			}
			else if (input->PushKey(DIK_LEFT))
			{
				block[selectBlock].Pos.x -= 1;
			}
			else if (input->PushKey(DIK_RIGHT))
			{
				block[selectBlock].Pos.x += 1;
			}
			else if (input->PushKey(DIK_BACK))
			{
				process = None;
			}
			object3d[selectBlock]->SetPosition({ block[selectBlock].Pos });
		}
		if (process == Rota)
		{
			if (input->PushKey(DIK_LEFT))
			{
				block[selectBlock].Rotation.z -= 1;
			}
			else if (input->PushKey(DIK_RIGHT))
			{
				block[selectBlock].Rotation.z += 1;
			}
			else if (input->PushKey(DIK_BACK))
			{
				process = None;
			}
			object3d[selectBlock]->SetRotation({ block[selectBlock].Rotation });
		}
	}

	//�J�����̏����������牺
	//�A�b�v�f�[�g�������牺
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
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
#pragma region �w�i�X�v���C�g�`��
	// �[�x�o�b�t�@�N���A
	dxCommon->ClearDepthBuffer();
#pragma endregion �w�i�X�v���C�g�`��
#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3D�I�u�N�W�F�N�g�̕`��
	for (int i = 0; i < _countof(object3d); i++)
	{
		object3d[i]->Draw();
	}
	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(dxCommon->GetCommandList());
	// �w�i�X�v���C�g�`��
	if (phase == Trimming)
	{
		sprite[0]->Draw();
		if (process == Size)
		{
			sprite[1]->Draw();
		}
		else if (process == Cut)
		{
			sprite[2]->Draw();
		}
	}
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
#pragma endregion 3D�I�u�W�F�N�g�`��
#pragma region �p�[�e�B�N��
	//particleEmitter->Draw(dxCommon->GetCommandList());
#pragma endregion �p�[�e�B�N��
#pragma region �O�i�X�v���C�g�`��
	// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(dxCommon->GetCommandList());
	// �f�o�b�O�e�L�X�g�̕`��
	debugText.DrawAll(dxCommon->GetCommandList());
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
#pragma endregion �O�i�X�v���C�g�`��
}