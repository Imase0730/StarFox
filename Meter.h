#pragma once

#include "Effects.h"
#include "VertexTypes.h"
#include "PrimitiveBatch.h"

class Meter
{
private:

	// パワーメーターのサイズ
	static const int METER_WIDTH = 400;
	static const int METER_HEIGHT = 20;

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_effect;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// ウインドウのサイズ
	int m_windowWidth, m_windowHeight;

	// パワー
	float m_power;

public:

	// コンストラクタ
	Meter(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);

	// デストラクタ
	~Meter();

	// 描画関数
	void Draw(ID3D11DeviceContext* context, const DirectX::CommonStates* states);

	// パワーの設定関数
	void SetPower(float power) { m_power = power; }

};

