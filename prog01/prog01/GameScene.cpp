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
	if (!Sprite::LoadTexture(1, L"Resources/APEX_01.png"))
	{
		assert(0);
	}

	// �w�i�X�v���C�g����
	sprite = Sprite::Create(1, { 0.0f,0.0f });
	sprite->SetSize({ 100.0f,100.0f });
	sprite->SetPosition({ 100.0f,100.0f });

	//.obj�̖��O���w�肵�ă��f����ǂݍ���
	modelFighter = Model::CreateFromObject("sphere", true);
	// 3D�I�u�W�F�N�g����
	object3d = Object3d::Create();
	// 3D�I�u�W�F�N�g�Ƀ��f�������蓖�Ă�
	object3d->SetModel(modelFighter);
	object3d->SetScale({ 3, 3, 3 });
	object3d->SetPosition({ 0,2,0 });

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
	audio->PlayWave("Resources/Alarm01.wav");

	// �J���������_���Z�b�g
	camera->SetTarget({ 0, 0, 0 });
	camera->SetEye({ 0,10,-15 });
}

void GameScene::Update()
{
	//�L�[��������Ă���Ƃ��̏���
	if (input->TriggerPadRight())
	{
		OutputDebugStringA("Hit 0\n");
	}

	////X���W�AY���W���w�肵�ĕ\��
	//debugText.Print("Hello,DirectX!!", 200, 100, 1.0f);
	////X���W�AY���W�A�k�ڂ��w�肵�ĕ\��
	//debugText.Print("Nihon Kogakuin", 200, 200, 2.0f);

	// �I�u�W�F�N�g�ړ�
	//if (input->PushPadStickUp() || input->PushPadStickDown() || input->PushPadStickRight() || input->PushPadStickLeft())
	//{
	//	// ���݂̍��W���擾
	//	XMFLOAT3 position = object3d->GetPosition();
	//
	//	// �ړ���̍��W���v�Z
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
	//	// ���W�̕ύX�𔽉f
	//	object3d->SetPosition(position);
	//}

	XMFLOAT3 rot = object3d->GetRotation();
	rot.y += 1.0f;
	object3d->SetRotation(rot);

	// �J�����ړ�
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
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
#pragma region �w�i�X�v���C�g�`��
	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(dxCommon->GetCommandList());
	// �w�i�X�v���C�g�`��
	sprite->Draw();
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
	// �[�x�o�b�t�@�N���A
	dxCommon->ClearDepthBuffer();
#pragma endregion �w�i�X�v���C�g�`��
#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3D�I�u�N�W�F�N�g�̕`��
	object3d->Draw();
	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
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