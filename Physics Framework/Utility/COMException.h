#pragma once
#ifndef COMEXCEPTION_H
#define COMEXCEPTION_H

#include <comdef.h>
#include "StringConverter.h"

#define COM_ERROR_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw COMException( hr, msg, __FILE__, __FUNCTION__, __LINE__ )

class COMException
{
public:
	COMException(
		HRESULT hr,
		const std::string& msg,
		const std::string& file,
		const std::string& function,
		int line)
	{
		_com_error error(hr);
		whatMsg = L"Message: " + StringConverter::StringToWide(msg) + L"\n";
		whatMsg += error.ErrorMessage();
		whatMsg += L"\nFile: " + StringConverter::StringToWide(file);
		whatMsg += L"\nFunction: " + StringConverter::StringToWide(function);
		whatMsg += L"\nLine: " + StringConverter::StringToWide(std::to_string(line));
	}
	const wchar_t* what() const
	{
		return whatMsg.c_str();
	}
private:
	std::wstring whatMsg;
};

#endif