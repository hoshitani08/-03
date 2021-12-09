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

class GameScene
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

private: // �ÓI�����o�ϐ�
	static const int debugTextTexNumber = 0;

public: // �����o�֐�
	// �R���X�g�N���^
	GameScene();
	// �f�X�g���N�^
	~GameScene();
	// ������
	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);
	// ���t���[������
	void Update();
	// �`��
	void Draw();

private: // �����o�ϐ�
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText debugText;
	XMFLOAT3 cameraPos = { 0,10,-50 };
	XMFLOAT3 objectSize[50] = {};
	enum Phase
	{
		Move, Trimming
	};
	enum Process
	{
		None, Size, Cut, Rota, Color
	};
	bool phase = Move;
	int process = None;
	int selectBlock = 1;
	// �Q�[���V�[���p
	Sprite* sprite = nullptr;
	//�I�u�W�F�N�g
	Model* modelFighter = nullptr;
	Model* modelCube = nullptr;
	Object3d* object3d[50] = {nullptr};
	//�p�[�e�B�N��
	ParticleManager* particleMan = nullptr;
	ParticleEmitter* particleEmitter = nullptr;
	//�J����
	Camera* camera = nullptr;
	//���C�g
	Light* light = nullptr;
	//�����蔻��(��)
	Sphere sphere;
	//�����蔻��(����)
	Plane plane;
	//�����蔻��(�O�p�`)
	Triangle triangle;
	//�����蔻��(���C)
	Ray ray;
};