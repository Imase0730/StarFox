#include "pch.h"
#include "GameScene.h"
#include "DDSTextureLoader.h"
#include "TestScene.h"

using namespace DirectX;

// 床のサイズ
const float GameScene::FLOOR_SIZE = 50.0f;

// 球ボールの半径
const float GameScene::BALL_RADIUS = 1.0f;

// ボールを置くエリアの大きさ
const float GameScene::BALL_PUT_AERA_SIZE = 45.0f;

// パワーの変化量
const float GameScene::AMOUNT_OF_CHANGE_OF_POWER = 0.01f;

// 1フレーム辺りの回転量
const float GameScene::FRAME_ROTATE_ANGLE = 1.0f;

// 摩擦係数
const float GameScene::FRACTION_RATE = 0.985f;

// 球の最大の速さ
const float GameScene::BALL_MAX_SPEED = 1.0f;

void GameScene::DrawFloor(ID3D11DeviceContext* context,
    const CommonStates* states,
    SimpleMath::Matrix world,
    SimpleMath::Matrix view,
    SimpleMath::Matrix proj)
{
    // エフェクトを適応する
    m_effect->SetWorld(world);
    m_effect->SetView(view);
    m_effect->SetProjection(proj);
    m_effect->SetTexture(m_floorTexture.Get());
    m_effect->Apply(context);

    // 入力レイアウトの設定
    context->IASetInputLayout(m_inputLayout.Get());

    // テクスチャサンプラの設定
    ID3D11SamplerState* sampler[] = { states->PointWrap() };
    context->PSSetSamplers(0, 1, sampler);

    // ブレンドステートの設定
    context->OMSetBlendState(states->Opaque(), nullptr, 0xffffffff);

    // 深度バッファの設定
    context->OMSetDepthStencilState(states->DepthDefault(), 0);

    // 床の頂点情報
    static const float halfFloorSize = FLOOR_SIZE / 2.0f;
    static VertexPositionTexture vertexes[] =
    {
        VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2(0.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3(halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2(halfFloorSize, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2(0.0f, halfFloorSize)),
        VertexPositionTexture(SimpleMath::Vector3(halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2(halfFloorSize, halfFloorSize)),
    };
    // 床のインデックス情報
    static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

    // 床を描画
    m_primitiveBatch->Begin();
    m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
    m_primitiveBatch->End();
}

void GameScene::Initialize()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();

    // ゲームの初期化
    InitializeGame();
}

void GameScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

    // キー入力を取得する
    auto kb = Keyboard::Get().GetState();
    auto tracker = GetUserResources()->GetKeyboardStateTracker();
    
    // シーンを変更
    if (tracker->pressed.A)
    {
        ChangeScene<TestScene>();
    }

    // パワーの設定
    m_power += AMOUNT_OF_CHANGE_OF_POWER;
    if (m_power > 1.0f) m_power = -1.0f;
    m_meter->SetPower(fabsf(m_power));

    // 左右キーでカメラの向きを回転する
    if (kb.Left)
    {
        m_ballInfo[BN_PLAYER].direction += XMConvertToRadians(FRAME_ROTATE_ANGLE);
    }
    if (kb.Right)
    {
        m_ballInfo[BN_PLAYER].direction -= XMConvertToRadians(FRAME_ROTATE_ANGLE);
    }

    // スペースキーが押された
    if (tracker->pressed.Space)
    {
        // ボールを打ち出す
        m_ballInfo[BN_PLAYER].speed = fabsf(m_power) * BALL_MAX_SPEED;
    }

    // すべての球を移動させる
    for (int i = 0; i < BN_NUM; i++)
    {
        SimpleMath::Matrix matrix = SimpleMath::Matrix::CreateRotationY(m_ballInfo[i].direction);
        SimpleMath::Vector3 vec = SimpleMath::Vector3(0.0f, 0.0f, m_ballInfo[i].speed);
        vec = SimpleMath::Vector3::Transform(vec, matrix);
        m_ballInfo[i].position += vec;
        m_ballInfo[i].speed *= FRACTION_RATE;
    }

    // 球同士の衝突判定
    for (int i = 0; i < BN_NUM - 1; i++)
    {
        for (int j = i + 1; j < BN_NUM; j++)
        {
            // 球と球の衝突判定
            SimpleMath::Vector3 v = m_ballInfo[i].position - m_ballInfo[j].position;
            if (v.LengthSquared() < (BALL_RADIUS * 2.0f) * (BALL_RADIUS * 2.0f))
            {
                // 衝突したので球の速度を設定する
                SetReflectionBallVelocity(&m_ballInfo[i], &m_ballInfo[j]);
            }
        }
    }

    // ゲームクリアチェック
    if (IsGameClear())
    {
        MessageBox(nullptr, L"GameClear", L"おめでとう!", MB_OK);
        InitializeGame();
    }

    // ゲームオーバーチェック
    if (IsGameOver())
    {
        MessageBox(nullptr, L"GameOver", L"残念", MB_OK);
        InitializeGame();
    }
 
}

void GameScene::Render()
{
    auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
    auto states = GetUserResources()->GetCommonStates();

    SimpleMath::Matrix world;

    static SimpleMath::Vector3 fromPt = SimpleMath::Vector3::Zero;	// カメラの位置
    SimpleMath::Vector3 lookAt = m_ballInfo[BN_PLAYER].position;	// 注視点 （もち球の位置）

    // もち球がほぼ停止していれば
    if (m_ballInfo[BN_PLAYER].speed < 0.01f)
    {
        // カメラの位置を更新する
        SimpleMath::Vector3 v(0.0f, 3.0f, -10.0f);
        SimpleMath::Matrix m = SimpleMath::Matrix::CreateRotationY(m_ballInfo[BN_PLAYER].direction);
        v = SimpleMath::Vector3::Transform(v, m);
        fromPt = lookAt + v;
    }
    // ビュー行列を作成
    m_view = SimpleMath::Matrix::CreateLookAt(fromPt, lookAt, SimpleMath::Vector3::UnitY);

    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // 床の描画
    DrawFloor(context, states, world, m_view, m_proj);

    // 球の影の描画
    DrawShadow(context, states);

    // 球の描画
    for (int i = 0; i < BN_NUM; i++)
    {
        world = SimpleMath::Matrix::CreateTranslation(m_ballInfo[i].position);
        m_ball->Draw(world, m_view, m_proj, m_ballInfo[i].color);
    }

    // パワーメーターの描画
    m_meter->Draw(context, states);
}

void GameScene::Finalize()
{
    m_meter.reset();
    m_shadowTexture.Reset();
    m_ball.reset();
    m_floorTexture.Reset();
    m_primitiveBatch.reset();
    m_inputLayout.Reset();
    m_effect.reset();
}

void GameScene::CreateDeviceDependentResources()
{
    auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
    auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

    // エフェクトの作成
    m_effect = std::make_unique<BasicEffect>(device);
    m_effect->SetLightingEnabled(false);
    m_effect->SetTextureEnabled(true);
    m_effect->SetVertexColorEnabled(false);

    // 入力レイアウトの作成
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionTexture>(device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );

    // プリミティブバッチの作成
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

    // テクスチャの読み込み
    CreateDDSTextureFromFile(device, L"Resources/Textures/floor.dds", nullptr, m_floorTexture.ReleaseAndGetAddressOf());

    // 球の作成
    m_ball = DirectX::GeometricPrimitive::CreateSphere(context, BALL_RADIUS * 2.0f);

    // テクスチャの読み込み
    CreateDDSTextureFromFile(device, L"Resources/Textures/shadow.dds", nullptr, m_shadowTexture.ReleaseAndGetAddressOf());

    // パワーメーターの作成
    m_meter = std::make_unique<Meter>(device, context, 800, 600);
}

void GameScene::CreateWindowSizeDependentResources()
{
}

void GameScene::OnDeviceLost()
{
    Finalize();
}

void GameScene::InitializeGame()
{
    // プレイヤーのもち球情報初期化
    m_ballInfo[BN_PLAYER].position.x = 0.0f;
    m_ballInfo[BN_PLAYER].position.y = 0.0f;
    m_ballInfo[BN_PLAYER].position.z = 0.0f;
    m_ballInfo[BN_PLAYER].direction = 0.0f;
    m_ballInfo[BN_PLAYER].speed = 0.0f;
    m_ballInfo[BN_PLAYER].color = Colors::White;

    // プレイヤーのもち球以外の球の初期化
    for (size_t i = 1; i < BN_NUM; i++)
    {
        // ランダムでボールの位置を設定
        m_ballInfo[i].position.x = static_cast<float>(rand() % static_cast<int>(BALL_PUT_AERA_SIZE)) - BALL_PUT_AERA_SIZE / 2.0f;
        m_ballInfo[i].position.y = 0.0f;
        m_ballInfo[i].position.z = static_cast<float>(rand() % static_cast<int>(BALL_PUT_AERA_SIZE)) - BALL_PUT_AERA_SIZE / 2.0f;
        m_ballInfo[i].direction = 0.0f;
        m_ballInfo[i].speed = 0.0f;

        // ランダムでボールの色を設定
        switch (static_cast<BallColor>(rand() % static_cast<int>(BallColor::NumItems)))
        {
        case BallColor::Yellow:
            m_ballInfo[i].color = Colors::Yellow;
            break;
        case BallColor::Magenta:
            m_ballInfo[i].color = Colors::Magenta;
            break;
        default:
            m_ballInfo[i].color = Colors::Cyan;
            break;
        }

    }
}

void GameScene::DrawShadow(ID3D11DeviceContext* context, const DirectX::CommonStates* states)
{
    // エフェクトを適応する
    m_effect->SetWorld(SimpleMath::Matrix::Identity);
    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);
    m_effect->SetTexture(m_shadowTexture.Get());
    m_effect->Apply(context);

    // 入力レイアウトの設定
    context->IASetInputLayout(m_inputLayout.Get());

    // テクスチャサンプラの設定
    ID3D11SamplerState* sampler[] = { states->LinearClamp() };
    context->PSSetSamplers(0, 1, sampler);

    // アルファブレンドの設定
    context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);

    // 深度バッファの設定
    context->OMSetDepthStencilState(states->DepthNone(), 0);

    // 影の頂点情報
    static VertexPositionTexture vertexes[] =
    {
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 1.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 1.0f)),
    };
    // 影のインデックス情報
    static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

    m_primitiveBatch->Begin();

    // すべての球の影を描画
    for (int i = 0; i < BN_NUM; i++)
    {
        // 影の表示位置の設定
        float x = m_ballInfo[i].position.x;
        float z = m_ballInfo[i].position.z;
        vertexes[0].position = SimpleMath::Vector3(-BALL_RADIUS + x, -1.0f, -BALL_RADIUS + z);
        vertexes[1].position = SimpleMath::Vector3(BALL_RADIUS + x, -1.0f, -BALL_RADIUS + z);
        vertexes[2].position = SimpleMath::Vector3(-BALL_RADIUS + x, -1.0f, BALL_RADIUS + z);
        vertexes[3].position = SimpleMath::Vector3(BALL_RADIUS + x, -1.0f, BALL_RADIUS + z);
        m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
    }

    m_primitiveBatch->End();
}

void GameScene::SetReflectionBallVelocity(BallInfo* ball_a, BallInfo* ball_b)
{
    // 衝突した球を反射させる（正しい計算はしていません）
    SimpleMath::Vector3 v = ball_b->position - ball_a->position;

    // 球の移動方向を設定
    ball_a->direction = atan2f(v.x, v.z) + XM_PI;
    ball_b->direction = ball_a->direction + XM_PI;

    v.Normalize();
    // 連続して衝突しないように球の位置を調整する
    ball_b->position = ball_a->position + (v * (BALL_RADIUS * 2.0f + 0.01f));
    // 球の速さをコピー
    ball_b->speed = ball_a->speed;
}

bool GameScene::IsGameClear()
{
    // もち球以外のすべて球が床の外へ出たか？
    for (int i = 0; i < BN_NUM; i++)
    {
        if (i == BN_PLAYER) continue;
        float x = m_ballInfo[i].position.x + (FLOOR_SIZE / 2.0f);
        float z = m_ballInfo[i].position.z + (FLOOR_SIZE / 2.0f);
        if (x >= 0.0f && x <= FLOOR_SIZE && z >= 0.0f && z <= FLOOR_SIZE)
        {
            return false;
        }
    }
    return true;
}

bool GameScene::IsGameOver()
{
    // プレイヤーのもち球が床の外へ出たか？
    float x = m_ballInfo[BN_PLAYER].position.x + (FLOOR_SIZE / 2.0f);
    float z = m_ballInfo[BN_PLAYER].position.z + (FLOOR_SIZE / 2.0f);
    if (x < 0.0f || x > FLOOR_SIZE || z < 0.0f || z > FLOOR_SIZE)
    {
        return true;
    }
    return false;
}
