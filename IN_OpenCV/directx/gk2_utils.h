#ifndef __GK2_UTILS_H_
#define __GK2_UTILS_H_

#include <d3d11.h>
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>

namespace gk2
{
	class Utils
	{
	public:
		static void COMRelease(IUnknown* comObject);
		static void DI8DeviceRelease(IDirectInputDevice8W* device);

		template<typename T>
		static void DeleteArray(T* arrayPtr)
		{
			if (arrayPtr != nullptr)
				delete [] arrayPtr;
		}

		//Alokowanie pami�ci wyr�wnanej do 16 bajt�w.
		//Pami�� tak zaalokowan� nale�y zwolnic za pomoc� metody Utils::Delete16Aligned.
		static void* New16Aligned(size_t size);
		static void Delete16Aligned(void* ptr);
	};
}

#endif __GK2_UTILS_H_
