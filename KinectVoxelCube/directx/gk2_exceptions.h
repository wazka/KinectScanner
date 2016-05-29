#ifndef __GK2_EXCEPTIONS_H_
#define __GK2_EXCEPTIONS_H_

#include <Windows.h>
#include <string>
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#define STRINGIFY(x) #x
#define TOWSTRING(x) WIDEN(STRINGIFY(x))
#define __AT__ __WFILE__ L":" TOWSTRING(__LINE__)

namespace gk2
{
	class Exception
	{
	public:
		explicit Exception(const wchar_t* location) { m_location = location; }
		virtual ~Exception() { }

		virtual std::wstring getMessage() const = 0;
		virtual int getExitCode() const = 0;
		const wchar_t* getErrorLocation() const { return m_location; }
	private:
		const wchar_t* m_location;
	};

	class WinAPIException : public Exception
	{
	public:
		explicit WinAPIException(const wchar_t* location, DWORD errorCode = GetLastError());
		int getExitCode() const override { return getErrorCode(); }
		DWORD getErrorCode() const { return m_code; }
		std::wstring getMessage() const override;

	private:
		DWORD m_code;
	};

	class CustomException : public Exception
	{
	public:
		CustomException(const std::wstring& message, const wchar_t* location);
		CustomException(std::wstring&& message, const wchar_t* location);
		int getExitCode() const override { return -1; }
		std::wstring getMessage() const override;

	private:
		std::wstring m_message;
	};
}

#define THROW_WINAPI throw gk2::WinAPIException(__AT__)
#define THROW(message) throw gk2::CustomException(message, __AT__)

#endif __GK2_EXCEPTIONS_H_
