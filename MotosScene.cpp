#include "pch.h"
#include "MotosScene.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

void MotosScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void MotosScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	//// �_��
	//m_counter += elapsedTime;
	//if (m_counter >= m_interval)
	//{
	//	m_counter = 0.0f;
	//	if (m_displayFlag)
	//	{
	//		m_displayFlag = false;
	//	}
	//	else
	//	{
	//		m_displayFlag = true;
	//	}
	//}

	// �X�N���[��
	m_scroll += 0.5f;
	if (m_scroll >= 100.0f) m_scroll -= 100.0f;

}

void MotosScene::Render()
{
	m_spriteBatch->Begin();

	// ���̃^�C���̕`��
	for (int j = 0; j < 9; j++)
	{
		for (int i = 0; i < 7; i++)
		{
			m_spriteBatch->Draw(m_tileTexture.Get(),
				SimpleMath::Vector2(i * 100.0f - m_scroll, (j - 1) * 100.0f + m_scroll));
		}
	}

	// �^�C�g���̕`��
	m_spriteBatch->Draw(m_titleTexture.Get(), m_fullscreenRect);

	m_spriteBatch->End();
}

void MotosScene::Finalize()
{
	m_spriteBatch.reset();
	m_pushSpaceTexture.Reset();
	m_tileTexture.Reset();
	m_titleTexture.Reset();
}

void MotosScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// �e�N�X�`���̓ǂݍ���
	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"Resources/Textures/title.dds", nullptr, m_titleTexture.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"Resources/Textures/title_tile.dds", nullptr, m_tileTexture.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"Resources/Textures/push_space.dds", nullptr, m_pushSpaceTexture.ReleaseAndGetAddressOf()));

	// �X�v���C�g�o�b�`�̍쐬
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
}

void MotosScene::CreateWindowSizeDependentResources()
{
	auto size = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_fullscreenRect = size;
}

void MotosScene::OnDeviceLost()
{
	Finalize();
}
