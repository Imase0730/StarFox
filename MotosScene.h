#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"
#include "SpriteBatch.h"

class MotosScene : public Imase::Scene<UserResources>
{

private:

	// テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_titleTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tileTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pushSpaceTexture;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// スクロール値
	float m_scroll;

	// フルスクリーンの矩形サイズ
	RECT m_fullscreenRect;

public:

	// コンストラクタ
	MotosScene() :m_scroll(0.0f), m_fullscreenRect{} {}

	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

	// デバイスに依存するリソースを作成する関数
	void CreateDeviceDependentResources() override;

	// ウインドウサイズに依存するリソースを作成する関数
	void CreateWindowSizeDependentResources() override;

	// デバイスロストした時に呼び出される関数
	void OnDeviceLost() override;
};

