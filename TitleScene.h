#pragma once

#include "ImaseLib/SceneManager.h"
#include "UserResources.h"

class TitleScene : public Imase::Scene<UserResources>
{
public:

	// ������
	void Initialize() override;

	// �X�V
	void Update(float elapsedTime) override;

	// �`��
	void Render() override;

	// �I������
	void Finalize() override;

	//// �f�o�C�X�Ɉˑ����郊�\�[�X���쐬����֐�
	//void CreateDeviceDependentResources() override;
	//// �E�C���h�E�T�C�Y�Ɉˑ����郊�\�[�X���쐬����֐�
	//void CreateWindowSizeDependentResources() override;
	//// �f�o�C�X���X�g�������ɌĂяo�����֐�
	//void OnDeviceLost() override;
};

