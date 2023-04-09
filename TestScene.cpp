#include "pch.h"
#include "TestScene.h"
#include "WICTextureLoader.h"

using namespace DirectX;

// ������
void TestScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// �X�V
void TestScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	auto timer = GetUserResources()->GetStepTimer();
	auto totalTime = static_cast<float>(timer->GetTotalSeconds());

	m_world = SimpleMath::Matrix::CreateRotationZ(totalTime / 2.f)
		* SimpleMath::Matrix::CreateRotationY(totalTime)
		* SimpleMath::Matrix::CreateRotationX(totalTime * 2.f);
}

// �`��
void TestScene::Render()
{
	m_spriteBatch->Begin();
	m_spriteBatch->Draw(m_background.Get(), m_fullscreenRect);
	m_spriteBatch->End();

	m_shape->Draw(m_world, m_view, m_projection);

	auto font = GetUserResources()->GetDebugFont();
	font->AddString(L"TestScene", SimpleMath::Vector2(0.0f, font->GetFontHeight()));
}

// �I��
void TestScene::Finalize()
{
}

// �f�o�C�X�Ɉˑ�����쐬����
void TestScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	DX::ThrowIfFailed(CreateWICTextureFromFile(device,
		L"Resources/Textures/sunset.jpg", nullptr,
		m_background.ReleaseAndGetAddressOf()));

	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
	m_shape = GeometricPrimitive::CreateTorus(context);

	m_view = SimpleMath::Matrix::CreateLookAt(
		SimpleMath::Vector3(0.f, 3.f, -3.f), SimpleMath::Vector3::Zero, SimpleMath::Vector3::UnitY);
}

// �E�C���h�E�T�C�Y�Ɉˑ�����쐬����
void TestScene::CreateWindowSizeDependentResources()
{
	auto size = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_fullscreenRect = size;

	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XM_PIDIV4, float(size.right) / float(size.bottom), 0.01f, 100.f);
}

// �f�o�C�X���X�g���̏���
void TestScene::OnDeviceLost()
{
	m_spriteBatch.reset();
	m_shape.reset();
	m_background.Reset();
}
