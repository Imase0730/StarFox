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

	// ���̍\����
	struct BallInfo
	{
		DirectX::SimpleMath::Vector3 position; 	// �ʒu
		float direction;                        // ����
		float speed;                            // ����
		DirectX::SimpleMath::Color color;       // �F
	};

	// ���̃T�C�Y
	static const float FLOOR_SIZE;

	// �v���C���[�̂������̔ԍ�
	static const int BN_PLAYER = 0;
	
	// ���̔��a
	static const float BALL_RADIUS;

	// ����u���G���A�̑傫��
	static const float BALL_PUT_AERA_SIZE;

	// �p���[�̕ω���
	static const float AMOUNT_OF_CHANGE_OF_POWER;

	// 1�t���[���ӂ�̉�]��
	static const float FRAME_ROTATE_ANGLE;
	
	// ���C�W��
	static const float FRACTION_RATE;
	
	// ���̍ő�̑���
	static const float BALL_MAX_SPEED;

	// ���̐F
	enum class BallColor
	{
		Yellow,
		Magenta,
		Cyan,
		NumItems
	};

	// �x�[�V�b�N�G�t�F�N�g
	std::unique_ptr<DirectX::BasicEffect> m_effect;

	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// �v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// ���̃e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floorTexture;

	// �r���[�s��
	DirectX::SimpleMath::Matrix m_view;

	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_proj;

	// ���̏��
	static const int BN_NUM = 20;		// ���̏�̃{�[���̐��i�v���C���[�̂��������܂ށj
	BallInfo m_ballInfo[BN_NUM];

	// ���̃��f��
	std::unique_ptr<DirectX::GeometricPrimitive> m_ball;

	// ���̉e�̃e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowTexture;

	// �p���[���[�^�[
	std::unique_ptr<Meter> m_meter;

	// �p���[
	float m_power;

private:

	// ���̕`��֐�
	void DrawFloor(ID3D11DeviceContext* context,
		const DirectX::CommonStates* states,
		DirectX::SimpleMath::Matrix world,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj);

	// �Q�[���̏������֐�
	void InitializeGame();

	// ���̉e�̕`��
	void DrawShadow(ID3D11DeviceContext* context, const DirectX::CommonStates* states);

	// �Փˎ��̂Q�̋��̔��˂��s���֐�
	void SetReflectionBallVelocity(BallInfo* ball_a, BallInfo* ball_b);

	// �Q�[���N���A�̃`�F�b�N�֐�
	bool IsGameClear();

	// �Q�[���I�[�o�[�̃`�F�b�N�֐�
	bool IsGameOver();

public:

	// �R���X�g���N�^
	GameScene() : m_power(0.0f) {}

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

