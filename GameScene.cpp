#include "pch.h"
#include "GameScene.h"
#include "DDSTextureLoader.h"
#include "TestScene.h"

using namespace DirectX;

// ���̃T�C�Y
const float GameScene::FLOOR_SIZE = 50.0f;

// ���{�[���̔��a
const float GameScene::BALL_RADIUS = 1.0f;

// �{�[����u���G���A�̑傫��
const float GameScene::BALL_PUT_AERA_SIZE = 45.0f;

// �p���[�̕ω���
const float GameScene::AMOUNT_OF_CHANGE_OF_POWER = 0.01f;

// 1�t���[���ӂ�̉�]��
const float GameScene::FRAME_ROTATE_ANGLE = 1.0f;

// ���C�W��
const float GameScene::FRACTION_RATE = 0.985f;

// ���̍ő�̑���
const float GameScene::BALL_MAX_SPEED = 1.0f;

void GameScene::DrawFloor(ID3D11DeviceContext* context,
    const CommonStates* states,
    SimpleMath::Matrix world,
    SimpleMath::Matrix view,
    SimpleMath::Matrix proj)
{
    // �G�t�F�N�g��K������
    m_effect->SetWorld(world);
    m_effect->SetView(view);
    m_effect->SetProjection(proj);
    m_effect->SetTexture(m_floorTexture.Get());
    m_effect->Apply(context);

    // ���̓��C�A�E�g�̐ݒ�
    context->IASetInputLayout(m_inputLayout.Get());

    // �e�N�X�`���T���v���̐ݒ�
    ID3D11SamplerState* sampler[] = { states->PointWrap() };
    context->PSSetSamplers(0, 1, sampler);

    // �u�����h�X�e�[�g�̐ݒ�
    context->OMSetBlendState(states->Opaque(), nullptr, 0xffffffff);

    // �[�x�o�b�t�@�̐ݒ�
    context->OMSetDepthStencilState(states->DepthDefault(), 0);

    // ���̒��_���
    static const float halfFloorSize = FLOOR_SIZE / 2.0f;
    static VertexPositionTexture vertexes[] =
    {
        VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2(0.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3(halfFloorSize, -1.0f, -halfFloorSize), SimpleMath::Vector2(halfFloorSize, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3(-halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2(0.0f, halfFloorSize)),
        VertexPositionTexture(SimpleMath::Vector3(halfFloorSize, -1.0f,  halfFloorSize), SimpleMath::Vector2(halfFloorSize, halfFloorSize)),
    };
    // ���̃C���f�b�N�X���
    static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

    // ����`��
    m_primitiveBatch->Begin();
    m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
    m_primitiveBatch->End();
}

void GameScene::Initialize()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();

    // �Q�[���̏�����
    InitializeGame();
}

void GameScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

    // �L�[���͂��擾����
    auto kb = Keyboard::Get().GetState();
    auto tracker = GetUserResources()->GetKeyboardStateTracker();
    
    // �V�[����ύX
    if (tracker->pressed.A)
    {
        ChangeScene<TestScene>();
    }

    // �p���[�̐ݒ�
    m_power += AMOUNT_OF_CHANGE_OF_POWER;
    if (m_power > 1.0f) m_power = -1.0f;
    m_meter->SetPower(fabsf(m_power));

    // ���E�L�[�ŃJ�����̌�������]����
    if (kb.Left)
    {
        m_ballInfo[BN_PLAYER].direction += XMConvertToRadians(FRAME_ROTATE_ANGLE);
    }
    if (kb.Right)
    {
        m_ballInfo[BN_PLAYER].direction -= XMConvertToRadians(FRAME_ROTATE_ANGLE);
    }

    // �X�y�[�X�L�[�������ꂽ
    if (tracker->pressed.Space)
    {
        // �{�[����ł��o��
        m_ballInfo[BN_PLAYER].speed = fabsf(m_power) * BALL_MAX_SPEED;
    }

    // ���ׂĂ̋����ړ�������
    for (int i = 0; i < BN_NUM; i++)
    {
        SimpleMath::Matrix matrix = SimpleMath::Matrix::CreateRotationY(m_ballInfo[i].direction);
        SimpleMath::Vector3 vec = SimpleMath::Vector3(0.0f, 0.0f, m_ballInfo[i].speed);
        vec = SimpleMath::Vector3::Transform(vec, matrix);
        m_ballInfo[i].position += vec;
        m_ballInfo[i].speed *= FRACTION_RATE;
    }

    // �����m�̏Փ˔���
    for (int i = 0; i < BN_NUM - 1; i++)
    {
        for (int j = i + 1; j < BN_NUM; j++)
        {
            // ���Ƌ��̏Փ˔���
            SimpleMath::Vector3 v = m_ballInfo[i].position - m_ballInfo[j].position;
            if (v.LengthSquared() < (BALL_RADIUS * 2.0f) * (BALL_RADIUS * 2.0f))
            {
                // �Փ˂����̂ŋ��̑��x��ݒ肷��
                SetReflectionBallVelocity(&m_ballInfo[i], &m_ballInfo[j]);
            }
        }
    }

    // �Q�[���N���A�`�F�b�N
    if (IsGameClear())
    {
        MessageBox(nullptr, L"GameClear", L"���߂łƂ�!", MB_OK);
        InitializeGame();
    }

    // �Q�[���I�[�o�[�`�F�b�N
    if (IsGameOver())
    {
        MessageBox(nullptr, L"GameOver", L"�c�O", MB_OK);
        InitializeGame();
    }
 
}

void GameScene::Render()
{
    auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
    auto states = GetUserResources()->GetCommonStates();

    SimpleMath::Matrix world;

    static SimpleMath::Vector3 fromPt = SimpleMath::Vector3::Zero;	// �J�����̈ʒu
    SimpleMath::Vector3 lookAt = m_ballInfo[BN_PLAYER].position;	// �����_ �i�������̈ʒu�j

    // ���������قڒ�~���Ă����
    if (m_ballInfo[BN_PLAYER].speed < 0.01f)
    {
        // �J�����̈ʒu���X�V����
        SimpleMath::Vector3 v(0.0f, 3.0f, -10.0f);
        SimpleMath::Matrix m = SimpleMath::Matrix::CreateRotationY(m_ballInfo[BN_PLAYER].direction);
        v = SimpleMath::Vector3::Transform(v, m);
        fromPt = lookAt + v;
    }
    // �r���[�s����쐬
    m_view = SimpleMath::Matrix::CreateLookAt(fromPt, lookAt, SimpleMath::Vector3::UnitY);

    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // ���̕`��
    DrawFloor(context, states, world, m_view, m_proj);

    // ���̉e�̕`��
    DrawShadow(context, states);

    // ���̕`��
    for (int i = 0; i < BN_NUM; i++)
    {
        world = SimpleMath::Matrix::CreateTranslation(m_ballInfo[i].position);
        m_ball->Draw(world, m_view, m_proj, m_ballInfo[i].color);
    }

    // �p���[���[�^�[�̕`��
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

    // �G�t�F�N�g�̍쐬
    m_effect = std::make_unique<BasicEffect>(device);
    m_effect->SetLightingEnabled(false);
    m_effect->SetTextureEnabled(true);
    m_effect->SetVertexColorEnabled(false);

    // ���̓��C�A�E�g�̍쐬
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionTexture>(device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );

    // �v���~�e�B�u�o�b�`�̍쐬
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

    // �e�N�X�`���̓ǂݍ���
    CreateDDSTextureFromFile(device, L"Resources/Textures/floor.dds", nullptr, m_floorTexture.ReleaseAndGetAddressOf());

    // ���̍쐬
    m_ball = DirectX::GeometricPrimitive::CreateSphere(context, BALL_RADIUS * 2.0f);

    // �e�N�X�`���̓ǂݍ���
    CreateDDSTextureFromFile(device, L"Resources/Textures/shadow.dds", nullptr, m_shadowTexture.ReleaseAndGetAddressOf());

    // �p���[���[�^�[�̍쐬
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
    // �v���C���[�̂�������񏉊���
    m_ballInfo[BN_PLAYER].position.x = 0.0f;
    m_ballInfo[BN_PLAYER].position.y = 0.0f;
    m_ballInfo[BN_PLAYER].position.z = 0.0f;
    m_ballInfo[BN_PLAYER].direction = 0.0f;
    m_ballInfo[BN_PLAYER].speed = 0.0f;
    m_ballInfo[BN_PLAYER].color = Colors::White;

    // �v���C���[�̂������ȊO�̋��̏�����
    for (size_t i = 1; i < BN_NUM; i++)
    {
        // �����_���Ń{�[���̈ʒu��ݒ�
        m_ballInfo[i].position.x = static_cast<float>(rand() % static_cast<int>(BALL_PUT_AERA_SIZE)) - BALL_PUT_AERA_SIZE / 2.0f;
        m_ballInfo[i].position.y = 0.0f;
        m_ballInfo[i].position.z = static_cast<float>(rand() % static_cast<int>(BALL_PUT_AERA_SIZE)) - BALL_PUT_AERA_SIZE / 2.0f;
        m_ballInfo[i].direction = 0.0f;
        m_ballInfo[i].speed = 0.0f;

        // �����_���Ń{�[���̐F��ݒ�
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
    // �G�t�F�N�g��K������
    m_effect->SetWorld(SimpleMath::Matrix::Identity);
    m_effect->SetView(m_view);
    m_effect->SetProjection(m_proj);
    m_effect->SetTexture(m_shadowTexture.Get());
    m_effect->Apply(context);

    // ���̓��C�A�E�g�̐ݒ�
    context->IASetInputLayout(m_inputLayout.Get());

    // �e�N�X�`���T���v���̐ݒ�
    ID3D11SamplerState* sampler[] = { states->LinearClamp() };
    context->PSSetSamplers(0, 1, sampler);

    // �A���t�@�u�����h�̐ݒ�
    context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xffffffff);

    // �[�x�o�b�t�@�̐ݒ�
    context->OMSetDepthStencilState(states->DepthNone(), 0);

    // �e�̒��_���
    static VertexPositionTexture vertexes[] =
    {
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 0.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(0.0f, 1.0f)),
        VertexPositionTexture(SimpleMath::Vector3::Zero, SimpleMath::Vector2(1.0f, 1.0f)),
    };
    // �e�̃C���f�b�N�X���
    static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

    m_primitiveBatch->Begin();

    // ���ׂĂ̋��̉e��`��
    for (int i = 0; i < BN_NUM; i++)
    {
        // �e�̕\���ʒu�̐ݒ�
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
    // �Փ˂������𔽎˂�����i�������v�Z�͂��Ă��܂���j
    SimpleMath::Vector3 v = ball_b->position - ball_a->position;

    // ���̈ړ�������ݒ�
    ball_a->direction = atan2f(v.x, v.z) + XM_PI;
    ball_b->direction = ball_a->direction + XM_PI;

    v.Normalize();
    // �A�����ďՓ˂��Ȃ��悤�ɋ��̈ʒu�𒲐�����
    ball_b->position = ball_a->position + (v * (BALL_RADIUS * 2.0f + 0.01f));
    // ���̑������R�s�[
    ball_b->speed = ball_a->speed;
}

bool GameScene::IsGameClear()
{
    // �������ȊO�̂��ׂċ������̊O�֏o�����H
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
    // �v���C���[�̂����������̊O�֏o�����H
    float x = m_ballInfo[BN_PLAYER].position.x + (FLOOR_SIZE / 2.0f);
    float z = m_ballInfo[BN_PLAYER].position.z + (FLOOR_SIZE / 2.0f);
    if (x < 0.0f || x > FLOOR_SIZE || z < 0.0f || z > FLOOR_SIZE)
    {
        return true;
    }
    return false;
}
