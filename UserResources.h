//--------------------------------------------------------------------------------------
// File: UserResources.h
//
// �V�[���֓n�����[�U�[���\�[�X�N���X�i�T���v���j
//
// Date: 2023.3.25
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#pragma once

#include "DeviceResources.h"
#include "ImaseLib/DebugFont.h"
#include "ImaseLib/DebugCamera.h"
#include "StepTimer.h"

// �e�V�[���ɓn�����ʃ��\�[�X���L�q���Ă�������
class UserResources
{
private:

	// �X�e�b�v�^�C�}�[
	DX::StepTimer* m_timer;

	// �f�o�C�X���\�[�X
	DX::DeviceResources* m_deviceResources;

	// �L�[�{�[�h�X�e�[�g�g���b�J�[
	DirectX::Keyboard::KeyboardStateTracker* m_tracker;

	// ���ʃX�e�[�g
	DirectX::CommonStates* m_states;

	// �f�o�b�O�t�H���g�ւ̃|�C���^
	Imase::DebugFont* m_debugFont;

	// �f�o�b�O�J�����ւ̃|�C���^
	Imase::DebugCamera* m_debugCamera;

public:

	// �R���X�g���N�^
	UserResources()
		: m_timer(nullptr)
		, m_deviceResources(nullptr)
		, m_states(nullptr)
		, m_debugFont(nullptr)
		, m_debugCamera(nullptr)
	{
	}

	// �X�e�b�v�^�C�}�[��ݒ肷��֐�
	void SetStepTimerStates(DX::StepTimer* timer) { m_timer = timer; }

	// �X�e�b�v�^�C�}�[���擾����֐�
	const DX::StepTimer* GetStepTimer() { return m_timer; }

	// �f�o�C�X���\�[�X��ݒ肷��֐�
	void SetDeviceResources(DX::DeviceResources* deviceResources) { m_deviceResources = deviceResources; }

	// �f�o�C�X���\�[�X���擾����֐�
	const DX::DeviceResources* GetDeviceResources() { return m_deviceResources; }

	// �L�[�{�[�h�X�e�[�g�g���b�J�[��ݒ肷��֐�
	void SetKeyboardStateTracker(DirectX::Keyboard::KeyboardStateTracker* tracker) { m_tracker = tracker; }

	// �L�[�{�[�h�X�e�[�g�g���b�J�[���擾����֐�
	const DirectX::Keyboard::KeyboardStateTracker* GetKeyboardStateTracker() { return m_tracker; }

	// ���ʃX�e�[�g��ݒ肷��֐�
	void SetCommonStates(DirectX::CommonStates* states) { m_states = states; }

	// ���ʃX�e�[�g���擾����֐�
	const DirectX::CommonStates* GetCommonStates() { return m_states; }

	// �f�o�b�O�t�H���g��ݒ肷��֐�
	void SetDebugFont(Imase::DebugFont* debugFont) { m_debugFont = debugFont; }

	// �f�o�b�O�t�H���g���擾����֐�
	Imase::DebugFont* GetDebugFont() { return m_debugFont; }

	// �f�o�b�O�J������ݒ肷��֐�
	void SetDebugCamera(Imase::DebugCamera* debugCamera) { m_debugCamera = debugCamera; }

	// �f�o�b�O�J�������擾����֐�
	Imase::DebugCamera* GetDebugCamera() { return m_debugCamera; }

};

