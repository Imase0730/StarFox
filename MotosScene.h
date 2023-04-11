#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"
#include "SpriteBatch.h"

class MotosScene : public Imase::Scene<UserResources>
{

private:

	// �e�N�X�`���n���h��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_titleTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tileTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pushSpaceTexture;

	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// �X�N���[���l
	float m_scroll;

	// �t���X�N���[���̋�`�T�C�Y
	RECT m_fullscreenRect;

public:

	// �R���X�g���N�^
	MotosScene() :m_scroll(0.0f), m_fullscreenRect{} {}

	// ������
	void Initialize() override;

	// �X�V
	void Update(float elapsedTime) override;

	// �`��
	void Render() override;

	// �I������
	void Finalize() override;

	// �f�o�C�X�Ɉˑ����郊�\�[�X���쐬����֐�
	void CreateDeviceDependentResources() override;

	// �E�C���h�E�T�C�Y�Ɉˑ����郊�\�[�X���쐬����֐�
	void CreateWindowSizeDependentResources() override;

	// �f�o�C�X���X�g�������ɌĂяo�����֐�
	void OnDeviceLost() override;
};

