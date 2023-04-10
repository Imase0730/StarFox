//--------------------------------------------------------------------------------------
// File: UserResources.h
//
// シーンへ渡すユーザーリソースクラス（サンプル）
//
// Date: 2023.3.25
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#pragma once

#include "DeviceResources.h"
#include "ImaseLib/DebugFont.h"
#include "ImaseLib/DebugCamera.h"
#include "StepTimer.h"

// 各シーンに渡す共通リソースを記述してください
class UserResources
{
private:

	// ステップタイマー
	DX::StepTimer* m_timer;

	// デバイスリソース
	DX::DeviceResources* m_deviceResources;

	// キーボードステートトラッカー
	DirectX::Keyboard::KeyboardStateTracker* m_tracker;

	// 共通ステート
	DirectX::CommonStates* m_states;

	// デバッグフォントへのポインタ
	Imase::DebugFont* m_debugFont;

	// デバッグカメラへのポインタ
	Imase::DebugCamera* m_debugCamera;

public:

	// コンストラクタ
	UserResources()
		: m_timer(nullptr)
		, m_deviceResources(nullptr)
		, m_states(nullptr)
		, m_debugFont(nullptr)
		, m_debugCamera(nullptr)
	{
	}

	// ステップタイマーを設定する関数
	void SetStepTimerStates(DX::StepTimer* timer) { m_timer = timer; }

	// ステップタイマーを取得する関数
	const DX::StepTimer* GetStepTimer() { return m_timer; }

	// デバイスリソースを設定する関数
	void SetDeviceResources(DX::DeviceResources* deviceResources) { m_deviceResources = deviceResources; }

	// デバイスリソースを取得する関数
	const DX::DeviceResources* GetDeviceResources() { return m_deviceResources; }

	// キーボードステートトラッカーを設定する関数
	void SetKeyboardStateTracker(DirectX::Keyboard::KeyboardStateTracker* tracker) { m_tracker = tracker; }

	// キーボードステートトラッカーを取得する関数
	const DirectX::Keyboard::KeyboardStateTracker* GetKeyboardStateTracker() { return m_tracker; }

	// 共通ステートを設定する関数
	void SetCommonStates(DirectX::CommonStates* states) { m_states = states; }

	// 共通ステートを取得する関数
	const DirectX::CommonStates* GetCommonStates() { return m_states; }

	// デバッグフォントを設定する関数
	void SetDebugFont(Imase::DebugFont* debugFont) { m_debugFont = debugFont; }

	// デバッグフォントを取得する関数
	Imase::DebugFont* GetDebugFont() { return m_debugFont; }

	// デバッグカメラを設定する関数
	void SetDebugCamera(Imase::DebugCamera* debugCamera) { m_debugCamera = debugCamera; }

	// デバッグカメラを取得する関数
	Imase::DebugCamera* GetDebugCamera() { return m_debugCamera; }

};

