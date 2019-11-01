#include "stdafx.h"
#include "VersionInfoFunctions.h"

//Windows includes.
#include <Knownfolders.h>
#include <shellapi.h>

//WiX includes.
#include <fileutil.h>
#include <memutil.h>
#include <shelutil.h>
#include <strutil.h>

HRESULT DAPI RaisedBar::RBSpeech::VersionInfoFunctions::ProductVersion(__in_z LPCWSTR wzFilename, __out DWORD* pdwVerMajor, __out DWORD* pdwVerMinor, __out DWORD* pdwVerBuild, __out DWORD* pdwVerQFE)
{
	DWORD dwVerMajor, dwVerMinor, dwVerBuild, dwVerQFE = 0;
	DWORD dwHandle = 0;
	UINT cbVerBuffer = 0;
	UINT cbTranslate = 0;
	LPVOID pVerBuffer = NULL;
	HRESULT hr = S_OK;
	LPWSTR szRequiredVersionInfoString = nullptr;
	LPWSTR szProductVersion = nullptr;
	UINT   uLen = 0;
	WORD wLanguage = 0;
	WORD  wCodePage = 0;
	UINT iRequiredTranslation = 0;
	LPWSTR* rgsczVersionComponents = nullptr;
	DWORD cVersionComponents = 0;
	hr = MemInitialize();
	ExitOnFailure(hr, "Unable to initialize the memory functions.");
	ExitOnNull(wzFilename, hr, S_FALSE, "The file path cannot be null.");
	ExitOnFalse(FileExistsEx(wzFilename, nullptr), hr, E_FILENOTFOUND, "The file was not found.");
	cbVerBuffer = GetFileVersionInfoSize(wzFilename, &dwHandle);
	if (cbVerBuffer == 0)
	{
		ExitOnLastErrorDebugTrace1(hr, "failed to get version info for file: %ls", wzFilename);
	}
	pVerBuffer = MemAlloc(cbVerBuffer, true);
	ExitOnNullDebugTrace1(pVerBuffer, hr, E_OUTOFMEMORY, "failed to allocate version info for file: %ls", wzFilename);

	if (!GetFileVersionInfo(wzFilename, dwHandle, cbVerBuffer, pVerBuffer))
	{
		ExitOnLastErrorDebugTrace1(hr, "failed to get version info for file: %ls", wzFilename);
	}

	hr = StrAllocStringSecure(&szRequiredVersionInfoString, L"\\VarFileInfo\\Translation", 0);
	ExitOnFailure(hr, "Unable to allocate the string.");
	ExitOnFalse(VerQueryValue(pVerBuffer, szRequiredVersionInfoString, (LPVOID*)&lpTranslate, &cbTranslate), hr, S_FALSE, "Unable to retrieve the list of available translations.");

	//Get the language code and code page for the first translation.
	wLanguage = lpTranslate[iRequiredTranslation].wLanguage;
	wCodePage = lpTranslate[iRequiredTranslation].wCodePage;
	hr = StrFree(szRequiredVersionInfoString);
	ExitOnFailure(hr, "Unable to free the string.");
	hr = StrAllocStringSecure(&szRequiredVersionInfoString, L"\\StringFileInfo\\", 0);
	ExitOnFailure(hr, "Unable to allocate the string.");
	hr = StrAllocConcatFormatted(&szRequiredVersionInfoString, L"%04x%04x", wLanguage, wCodePage);
	ExitOnFailure(hr, "Unable reallocate the string.");
	hr = StrAllocConcat(&szRequiredVersionInfoString, L"\\ProductVersion", 0);
	ExitOnFailure(hr, "Unable to extend the string.");
	ExitOnFalse(VerQueryValue(pVerBuffer, szRequiredVersionInfoString, (LPVOID*)&szProductVersion, &uLen), hr, S_FALSE, "Unable to retrieve the required version info string.");
	hr = StrSplitAllocArray(&rgsczVersionComponents, reinterpret_cast<UINT*>(&cVersionComponents), szProductVersion, L".");
	ExitOnFailure(hr, "Failed to split the returned product version in to it's components.");
	//Extract the major version.
	if (pdwVerMajor && cVersionComponents >= 1)
	{
		dwVerMajor = wcstoul(rgsczVersionComponents[0], nullptr, 0);
		*pdwVerMajor = dwVerMajor;
	}

	//Extract the minor version.
	if (pdwVerMinor && cVersionComponents >= 2)
	{
		dwVerMinor = wcstoul(rgsczVersionComponents[1], nullptr, 0);
		*pdwVerMinor = dwVerMinor;
	}

	//Extract the build version.
	if (pdwVerBuild && cVersionComponents >= 3)
	{
		dwVerBuild = wcstoul(rgsczVersionComponents[2], nullptr, 0);
		*pdwVerBuild = dwVerBuild;
	}

	//Extract the QFE version.
	if (pdwVerQFE && cVersionComponents >= 4)
	{
		dwVerQFE = wcstoul(rgsczVersionComponents[3], nullptr, 0);
		*pdwVerQFE = dwVerQFE;
	}

LExit:
	ReleaseMem(pVerBuffer);
	MemUninitialize();
	ReleaseNullStrSecure(szRequiredVersionInfoString);
	return hr;
}