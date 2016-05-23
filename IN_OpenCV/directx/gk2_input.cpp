#include "gk2_input.h"
#include "gk2_utils.h"

using namespace std;
using namespace gk2;

bool Keyboard::GetState(KeyboardState& state) const
{
	return DeviceBase::GetState(KeyboardState::STATE_TAB_LENGTH*sizeof(BYTE), reinterpret_cast<void*>(&state.m_keys));
}

bool Mouse::GetState(MouseState& state) const
{
	return DeviceBase::GetState(sizeof(DIMOUSESTATE), reinterpret_cast<void*>(&state.m_state));
}

shared_ptr<IDirectInputDevice8W> InputHelper::CreateInputDevice(HWND hWnd, const GUID& deviceGuid,
	const DIDATAFORMAT& dataFormat) const
{
	IDirectInputDevice8W* d;
	HRESULT result = m_inputObject->CreateDevice(deviceGuid, &d, 0);
	shared_ptr<IDirectInputDevice8W> device(d, Utils::DI8DeviceRelease);
	if (FAILED(result))
		THROW_WINAPI;
	result = device->SetDataFormat(&dataFormat);
	if (FAILED(result))
		THROW_WINAPI;
	result = device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		THROW_WINAPI;
	return device;
}
