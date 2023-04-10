#include "pch.h"
#include "Meter.h"
#include "DirectXHelpers.h"
#include "SimpleMath.h"

using namespace DirectX;

Meter::Meter(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height)
	: m_windowWidth(width)
	, m_windowHeight(height)
	, m_power(0.0f)
{
	// �G�t�F�N�g�̍쐬
	m_effect = std::make_unique<BasicEffect>(device);
	m_effect->SetLightingEnabled(false);
	m_effect->SetTextureEnabled(false);
	m_effect->SetVertexColorEnabled(true);

	// ���̓��C�A�E�g�̍쐬
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf())
	);

	// �v���~�e�B�u�o�b�`�̍쐬
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);
}

Meter::~Meter()
{
	m_primitiveBatch.reset();
	m_inputLayout.Reset();
	m_effect.reset();
}

void Meter::Draw(ID3D11DeviceContext* context, const DirectX::CommonStates* states)
{
	// �p���[���[�^�[�̒��_���
	static VertexPositionColor vertexes[] =
	{
		VertexPositionColor(SimpleMath::Vector3(0.0f,  0.0f, 0.0f), Colors::Red),
		VertexPositionColor(SimpleMath::Vector3(0.0f,  0.0f, 0.0f), Colors::Blue),
		VertexPositionColor(SimpleMath::Vector3(0.0f, METER_HEIGHT, 0.0f), Colors::Red),
		VertexPositionColor(SimpleMath::Vector3(0.0f, METER_HEIGHT, 0.0f), Colors::Blue),
	};
	// �p���[���[�^�[�̃C���f�b�N�X���
	static uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

	// �E�[�̂Q�̒��_���ړ�������
	vertexes[1].position.x = vertexes[3].position.x = fabsf(m_power) * METER_WIDTH;

	SimpleMath::Matrix world, view, proj;

	// �QD�`��p�̐��ˉe�s����쐬����
	proj = SimpleMath::Matrix::CreateOrthographicOffCenter(
		0.0f, static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight), 0.0f,
		0.0f, 1.0f);

	// �G�t�F�N�g��K������
	m_effect->SetWorld(world);
	m_effect->SetView(view);
	m_effect->SetProjection(proj);
	m_effect->Apply(context);

	// ���̓��C�A�E�g�̐ݒ�
	context->IASetInputLayout(m_inputLayout.Get());

	// �A���t�@�u�����h�̐ݒ�
	context->OMSetBlendState(states->Opaque(), nullptr, 0xffffffff);

	// �[�x�o�b�t�@�̐ݒ�
	context->OMSetDepthStencilState(states->DepthNone(), 0);

	// �p���[���[�^�[��`��
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, _countof(indexes), vertexes, _countof(vertexes));
	m_primitiveBatch->End();
}
