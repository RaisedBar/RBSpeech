#ifndef VERSION_INFO_FUNCTIONS_H
#define VERSION_INFO_FUNCTIONS_H
#pragma once
#include "stdafx.h"
//WiX includes.
#include <dutil.h>
#include "WiXMacros.h"

namespace RaisedBar
{
	namespace RBSpeech
	{
		namespace VersionInfoFunctions
		{
			namespace
			{
				// Structure used to store enumerated languages and code pages.

				struct LANGANDCODEPAGE {
					WORD wLanguage;
					WORD wCodePage;
				} *lpTranslate;
			}
			HRESULT DAPI ProductVersion(__in_z LPCWSTR wzFilename, __out DWORD* pdwVerMajor, __out DWORD* pdwVerMinor, __out DWORD* pdwVerBuild, __out DWORD* pdwVerQFE);
		}
	}
}
#endif // !OS_FUNCTIONS_H