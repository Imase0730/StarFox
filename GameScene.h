#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"
#include "Effects.h"
#include "VertexTypes.h"
#include "DirectXHelpers.h"
#include "PrimitiveBatch.h"
#include "GeometricPrimitive.h"
#include "Meter.h"

class GameScene : public Imase::Scene<UserResources>
{
private:

	// 球の構造体
	struct BallInfo
	{
		DirectX::SimpleMath::Vector3 position; 	// 位置
		float direction;                        // 方向
		float speed;                            // 速さ
		DirectX::SimpleMath::Color color;       // 色
	};

	// 床のサイズ
	static const float FLOOR_SIZE;

	// プレイヤーのもち球の番号
	static const int BN_PLAYER = 0;
	
	// 球の半径
	static const float BALL_RADIUS;

	// 球を置くエリアの大きさ
	static const float BALL_PUT_AERA_SIZE;

	// パワーの変化量
	static const float AMOUNT_OF_CHANGE_OF_POWER;

	// 1フレーム辺りの回転量
	static const float FRAME_ROTATE_ANGLE;
	
	// 摩擦係数
	static const float FRACTION_RATE;
	
	// 球の最大の速さ
	static const float BALL_MAX_SPEED;

	// 球の色
	enum class BallColor
	{
		Yellow,
		Magenta,
		Cyan,
		NumItems
	};

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_effect;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// 床のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floorTexture;

	// ビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// 射影行列
	DirectX::SimpleMath::Matrix m_proj;

	// 球の情報
	static const int BN_NUM = 20;		// 床の上のボールの数（プレイヤーのもち球を含む）
	BallInfo m_ballInfo[BN_NUM];

	// 球のモデル
	std::unique_ptr<DirectX::GeometricPrimitive> m_ball;

	// 球の影のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;

	// パワーメーター
	std::unique_ptr<Meter> m_meter;

	// パワー
	float m_power;

private:

	// 床の描画関数
	void DrawFloor(ID3D11DeviceContext* context,
		const DirectX::CommonStates* states,
		DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj);

	// ゲームの初期化関数
	void InitializeGame();

	// 球の影の描画
	void DrawShadow(ID3D11DeviceContext* context, const DirectX::CommonStates* states);

	// 衝突時の２つの球の反射を行う関数
	void SetReflectionBallVelocity(BallInfo* ball_a, BallInfo* ball_b);

	// ゲームクリアのチェック関数
	bool IsGameClear();

	// ゲームオーバーのチェック関数
	bool IsGameOver();

public:

	// コンストラクタ
	GameScene() : m_power(0.0f) {}

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

