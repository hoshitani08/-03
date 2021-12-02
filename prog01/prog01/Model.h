#pragma once

#include <string>
#include <unordered_map>
#include "Mesh.h"

class Model
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // 静的メンバ関数
	// 静的初期化
	static void StaticInitialize(ID3D12Device* device);

	static Model* CreateFromObject(const std::string& text, bool smoothing = false);

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;

private:// 静的メンバ関数
	//マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	// マテリアル登録
	void AddMaterial(Material* material);
	// デスクリプタヒープの初期化
	void CreateDescriptorHeap();
	// テクスチャ読み込み
	void LoadTexture();

public: // メンバ関数
	//初期化
	void Initialize(const std::string& text, bool smoothing);
	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

private: // メンバ変数
	// 名前
	std::string name;
	// メッシュコンテナ
	std::vector<Mesh*> meshes;
	// マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials;
	// デフォルトマテリアル
	Material* defaultMaterial = nullptr;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
};