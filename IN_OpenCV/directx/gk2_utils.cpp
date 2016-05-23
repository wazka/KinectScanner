#include "gk2_utils.h"

using namespace gk2;

void Utils::COMRelease(IUnknown* comObject)
{
	if (comObject != nullptr)
		comObject->Release();
}

void Utils::DI8DeviceRelease(IDirectInputDevice8W* device)
{
	if (device != nullptr)
	{
		device->Unacquire();
		device->Release();
	}
}

void* Utils::New16Aligned(size_t size)
{
	// ReSharper disable once CppNonReclaimedResourceAcquisition
	auto ptr = new BYTE[size + 16];
	auto shifted = ptr + 16;
	auto missalignment = static_cast<BYTE>(reinterpret_cast<LONG_PTR>(shifted) & 0xf);
	shifted = reinterpret_cast<BYTE*>(reinterpret_cast<LONG_PTR>(shifted) &~static_cast<LONG_PTR>(0xf));
	shifted[-1] = missalignment;
	return static_cast<void*>(shifted);
}

void Utils::Delete16Aligned(void* ptr)
{
	auto shifted = static_cast<BYTE*>(ptr);
	auto missalignment = shifted[-1];
	shifted = reinterpret_cast<BYTE*>(reinterpret_cast<LONG_PTR>(shifted) | static_cast<LONG_PTR>(missalignment));
	auto original = shifted - 16;
	delete [] original;
}