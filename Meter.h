#pragma once

#include "Effects.h"
#include "VertexTypes.h"
#include "PrimitiveBatch.h"

class Meter
{
private:

	// �p���[���[�^�[�̃T�C�Y
	static const int METER_WIDTH = 400;
	static const int METER_HEIGHT = 20;

	// �x�[�V�b�N�G�t�F�N�g
	std::unique_ptr<DirectX::BasicEffect> m_effect;

	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// �v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// �E�C���h�E�̃T�C�Y
	int m_windowWidth, m_windowHeight;

	// �p���[
	float m_power;

public:

	// �R���X�g���N�^
	Meter(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);

	// �f�X�g���N�^
	~Meter();

	// �`��֐�
	void Draw(ID3D11DeviceContext* context, const DirectX::CommonStates* states);

	// �p���[�̐ݒ�֐�
	void SetPower(float power) { m_power = power; }

};

