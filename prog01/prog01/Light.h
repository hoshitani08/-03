#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>

class Light
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

public: // サブクラス
	// 定数バッファ用データ構造体B0
	struct ConstBufferData
	{
		XMVECTOR lightv; //ライトへの方向を表すベクトル
		XMFLOAT3 lightcolor; //ライトの色
	};

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;

public: // 静的メンバ関数
	// 静的初期化
	static void StaticInitialize(ID3D12Device* device);
	//インスタンス生成
	static Light* Create();

private: // メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//ライト光線方向(単位ベクトル)
	XMVECTOR lightdir = { 1, 0, 0, 0 };
	//ライト色
	XMFLOAT3 lightcolor = { 1, 1, 1 };
	//ダーティフラグ
	bool dirty = false;

public: // メンバ関数
	//初期化
	void Initialize();
	//定数バッファ転送
	void TransferConstBuffer();
	//ライト方向をセット
	void SetLightDir(const XMVECTOR lightdir);
	//ライトの色をセット
	void SetLightColor(const XMFLOAT3 lightcolor);
	//更新
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);
};