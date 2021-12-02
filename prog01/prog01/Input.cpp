#include "Input.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

bool Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result = S_FALSE;

	//初期化（一度だけ行う処理）
	result = DirectInput8Create
	(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

#pragma region キーボード
	//キーボードデバイスの生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//入力データ形式のセット
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//排他制御レベルのセット
	result = devkeyboard->SetCooperativeLevel
	(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
#pragma endregion
#pragma region マウス
	// マウスデバイスの生成	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	// 入力データ形式のセット
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // 標準形式
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	// 排他制御レベルのセット
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
#pragma endregion
#pragma region ゲームパッド
	//初期化（一度だけ行う処理）
	result = DirectInput8Create
	(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinputPad, nullptr
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	// デバイスの列挙
	if (FAILED(dinputPad->EnumDevices(DI8DEVTYPE_GAMEPAD, DeviceFindCallBack, &parameter, DIEDFL_ATTACHEDONLY)))
	{
		assert(0);
		return result;
	}

	result = dinputPad->CreateDevice(GUID_Joystick, &devGamePad, NULL);
	if (FAILED(result))
	{
		padFlag = false;
	}

	if (padFlag == true)
	{
		// デバイスのフォーマットの設定
		result = devGamePad->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		// 軸モードを絶対値モードとして設定
		//ZeroMemory(&diprop, sizeof(diprop));
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.diph.dwObj = 0;
		diprop.dwData = DIPROPAXISMODE_ABS;	// 絶対値モードの指定(DIPROPAXISMODE_RELにしたら相対値)
		// 軸モードを変更
		result = devGamePad->SetProperty(DIPROP_AXISMODE, &diprop.diph);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		// X軸の値の範囲設定
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.diph.dwObj = DIJOFS_X;
		diprg.lMin = -1000;
		diprg.lMax = 1000;
		result = devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		// Y軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_Y;
		result = devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		// Z軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_Z;
		result = devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		// RX軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_RX;
		result = devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		// RY軸の値の範囲設定
		diprg.diph.dwObj = DIJOFS_RY;
		result = devGamePad->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		result = devGamePad->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}
	}
#pragma endregion

	return true;
}

void Input::Update()
{
	HRESULT result;

#pragma region キーボード
	//キーボード情報の取得開始
	result = devkeyboard->Acquire();
	// 前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));
	//全キーの入力状態を取得する
	result = devkeyboard->GetDeviceState(sizeof(key), key);
#pragma endregion
#pragma region マウス
	//マウス情報の取得開始
	result = devMouse->Acquire();
	// 前回のキー入力を保存
	memcpy(&mouseStatePre, &mouseState, sizeof(mouseState));
	//マウスの入力状態を取得する
	result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
#pragma endregion
#pragma region ゲームパッド
	if (padFlag == true)
	{
		// 制御開始
		result = devGamePad->Acquire();
		// 前回のキー入力を保存
		memcpy(&padDataPre, &padData, sizeof(padData));
		// ゲームパッドの入力情報取得
		result = devGamePad->GetDeviceState(sizeof(padData), &padData);
	}
#pragma endregion
}

bool Input::PushKey(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	// 0でなければ押している
	if (key[keyNumber])
	{
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// 異常な引数を検出
	assert(0 <= keyNumber && keyNumber <= 256);

	// 前回が0で、今回が0でなければトリガー
	if (!keyPre[keyNumber] && key[keyNumber])
	{
		return true;
	}

	// トリガーでない
	return false;
}

BOOL CALLBACK Input::DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	return DIENUM_CONTINUE;
}

bool Input::PushPadStickUp()
{
	if (padData.lY < -angle)
	{
		return true;
	}

	return false;
}

bool Input::PushPadStickDown()
{
	if (padData.lY > angle)
	{
		return true;
	}

	return false;
}

bool Input::PushPadStickRight()
{
	if (padData.lX > angle)
	{
		return true;
	}

	return false;
}

bool Input::PushPadStickLeft()
{
	if (padData.lX < -angle)
	{
		return true;
	}

	return false;
}

bool Input::TriggerPadLeft()
{
	if (padData.lZ > angle)
	{
		return true;
	}

	return false;
}

bool Input::TriggerPadRight()
{
	if (padData.lZ < -angle)
	{
		return true;
	}

	return false;
}

bool Input::PushPadKey(PadKey keyNumber)
{
	// 0でなければ押している
	if (padData.rgbButtons[keyNumber])
	{
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerPadKey(PadKey keyNumber)
{
	// 前回が0で、今回が0でなければトリガー
	if (!padDataPre.rgbButtons[keyNumber] && padData.rgbButtons[keyNumber])
	{
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::PushMouseLeft()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[0])
	{
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouseMiddle()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[2])
	{
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouseRight()
{
	// 0でなければ押している
	if (mouseState.rgbButtons[1])
	{
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerMouseLeft()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[0] && mouseState.rgbButtons[0])
	{
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::TriggerMouseMiddle()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[2] && mouseState.rgbButtons[2])
	{
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::TriggerMouseRight()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre.rgbButtons[1] && mouseState.rgbButtons[1])
	{
		return true;
	}

	// トリガーでない
	return false;
}

Input::MouseMove Input::GetMouseMove()
{
	MouseMove tmp;
	tmp.lX = mouseState.lX;
	tmp.lY = mouseState.lY;
	tmp.lZ = mouseState.lZ;
	return tmp;
}