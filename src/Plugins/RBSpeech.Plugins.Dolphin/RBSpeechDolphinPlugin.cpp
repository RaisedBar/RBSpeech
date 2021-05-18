#include "stdafx.h"
#include "RBSpeechDolphinPlugin.h"

//WiX includes.
#include <memutil.h>
#include <pathutil.h>
#include <procutil.h>
#include <strutil.h>

//C++ standard includes.
#include <filesystem>

using namespace std;
using namespace std::filesystem;

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsPluginForAnAssistiveTechnology()
{
	return S_OK;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsProductActive()
{
	HRESULT hr = S_OK;
	DWORD dwCurrentDolphinProduct = 0;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");
	hr = GetActualDolphinProduct(&dwCurrentDolphinProduct);
	ExitOnSpecificValue(dwCurrentDolphinProduct, 0, hr, S_FALSE, "A dolphin product is not running.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::Silence()
{
	HRESULT hr = S_OK;
	DWORD dwDolphinFunctionReturnCode = 0;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");
	hr = CanDolphinProductSpeak();
	ExitOnFailure(hr, "The currently active dolphin product is unable to stop speaking.");
	dwDolphinFunctionReturnCode = DolAccess_Action(DOLACTION_SILENCE);
	ExitIfValueGreaterThanSuppliedValue(dwDolphinFunctionReturnCode, DOLACCESS_SUCCESS, hr, S_FALSE, "Unable to silence the current dolphin product.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	HRESULT hr = S_OK;
	DWORD dwDolphinFunctionReturnCode = 0;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be spoken was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "The message to be spoken was an empty string.");
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");
	hr = CanDolphinProductSpeak();
	ExitOnFailure(hr, "The currently active Dolphin product is unable to produce speech.");

	if (silence == VARIANT_TRUE)
	{
		//Silence any existing speech.
		hr = Silence();
		ExitOnFailure(hr, "Unable to silence existing speech.");
	}

	dwDolphinFunctionReturnCode = DolAccess_Command((VOID *)text, SysStringLen(text), DOLAPI_COMMAND_SPEAK);
	ExitIfValueGreaterThanSuppliedValue(dwDolphinFunctionReturnCode, DOLACCESS_SUCCESS, hr, S_FALSE, "Unable to speak text through the current Dolphin product.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::BrailleText(BSTR text)
{
	UNREFERENCED_PARAMETER(text);
	return E_NOTIMPL;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsAPILoaded()
{
	return isAPILoaded;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::LoadAPI()
{
	HRESULT hr = S_OK;
	LPWSTR sczCurrentPath = NULL;
	path DolphinDllFileName;
	hr = IsAPILoaded();
	ExitOnSuccess(hr, "The Dolphin API is already loaded.");

	hr = PathForCurrentProcess(&sczCurrentPath, NULL);
	ExitOnFailure(hr, "Failed to get current process path.");
	DolphinDllFileName = sczCurrentPath;

	if (DolphinDllFileName.has_filename())
	{
		DolphinDllFileName.remove_filename();
	}

	//Add the Dolphin API file name.
	DolphinDllFileName /= L"dolapi.dll";

	//Check file existence.
	ExitOnFalse(exists(DolphinDllFileName), hr, E_FILENOTFOUND, "The Dolphin dll file has not been found.");

	//The dll file exists, so try to load it.
	DolphinDllApi.load(DolphinDllFileName.generic_wstring());
	ExitOnFalse(DolphinDllApi.is_loaded(), hr, S_FALSE, "The Dolphin dll file could not be loaded.");

	//Load the functions we need.
	ExitOnFalse(DolphinDllApi.has("DolAccess_GetSystem"), hr, S_FALSE, "The Dolphin dll does not export the DolAccess_GetSystem function.");
	DolAccess_GetSystem = DolphinDllApi.get<DolAccess_GetSystemFunc>("DolAccess_GetSystem");

	ExitOnFalse(DolphinDllApi.has("DolAccess_Action"), hr, S_FALSE, "The Dolphin dll does not export the DolAccess_Action function.");
	DolAccess_Action = DolphinDllApi.get<DolAccess_ActionFunc>("DolAccess_Action");

	ExitOnFalse(DolphinDllApi.has("DolAccess_Command"), hr, S_FALSE, "The Dolphin dll does not export the DolAccess_Command function.");
	DolAccess_Command = DolphinDllApi.get<DolAccess_CommandFunc>("DolAccess_Command");
	isAPILoaded = S_OK;
LExit:
	ReleaseStr(sczCurrentPath);
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::UnloadAPI()
{
	HRESULT hr = S_OK;
	hr = IsAPILoaded();
	ExitOnFailure(hr, "The Dolphin API is not loaded.");

	ExitOnNull(DolAccess_GetSystem, hr, S_FALSE, "The DolAccess_GetSystem function is already null.");
	DolAccess_GetSystem = nullptr;
	ExitOnNotNull(DolAccess_GetSystem, hr, S_FALSE, "The DolAccess_GetSystem function could not be released.");

	ExitOnNull(DolAccess_Action, hr, S_FALSE, "The DolAccess_Action function is already null.");
	DolAccess_Action = nullptr;
	ExitOnNotNull(DolAccess_Action, hr, S_FALSE, "The DolAccess_Action function could not be released.");

	ExitOnNull(DolAccess_Command, hr, S_FALSE, "The DolAccess_Command function is already null.");
	DolAccess_Command = nullptr;
	ExitOnNotNull(DolAccess_Command, hr, S_FALSE, "The DolAccess_Command function could not be released.");

	ExitOnFalse(DolphinDllApi.is_loaded(), hr, S_FALSE, "The Dolphin dll file is not loaded.");
	DolphinDllApi.unload();
	ExitOnTrue(DolphinDllApi.is_loaded(), hr, S_FALSE, "The Dolphin dll is still loaded.");
	isAPILoaded = S_FALSE;
LExit:
	return hr;
}

optional<wstring> RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::GetAssistiveTechnologyExecutable()
{
	optional<wstring> oResult = nullopt;
	HRESULT hr = S_OK;
	DWORD* prgProcessIds = NULL;
	DWORD cProcessIds = 0;
	hr = IsProductActive();
	ExitOnFailure(hr, "A dolphin product is not active.");
	for(auto wzPotentialDolphinProcessName : dolphinProcessNames)
	{
		ReleaseNullMem(prgProcessIds);
		cProcessIds = 0;
		hr = ProcFindAllIdsFromExeName(wzPotentialDolphinProcessName, &prgProcessIds, &cProcessIds);
		ExitOnFailure(hr, "Failed to enumerate all the processes by name %ls.", wzPotentialDolphinProcessName);
		if (SUCCEEDED(hr) && (cProcessIds >0))
		{
		//We have found a dolphon process, so exit.
			oResult = wzPotentialDolphinProcessName;
			break;
			goto LExit;
		}
	}
	
	LExit:
	ReleaseNullMem(prgProcessIds);
	cProcessIds = 0;
	return oResult;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::CanDolphinProductSpeak()
{
	HRESULT hr = S_OK;
	DWORD dwCurrentDolphinProduct = 0;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");

	hr = GetActualDolphinProduct(&dwCurrentDolphinProduct);
	ExitOnFailure(hr, "The current Dolphin product could not be obtained.");

	hr = IsHalActive() == S_OK || IsLunarPlusActive() == S_OK || IsSuperNovaActive() == S_OK;
		ExitOnFailure(hr, "The currently active Dolphin product is not able to speak.");
		LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::GetActualDolphinProduct(__out DWORD *pdwActualProduct)
{
	HRESULT hr = S_OK;
	DWORD currentDolphinProduct = 0;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");

	currentDolphinProduct = DolAccess_GetSystem();
	*pdwActualProduct = currentDolphinProduct;
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsHalActive()
{
	HRESULT hr = S_OK;
	DWORD dwCurrentDolphinProduct = 0;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");

	hr = GetActualDolphinProduct(&dwCurrentDolphinProduct);
	ExitOnSpecificValue(dwCurrentDolphinProduct, 0, hr, S_FALSE, "A dolphin product is not running.");

	hr = IsSpecificDolphinProductActive(dwCurrentDolphinProduct, DOLACCESS_HAL);
	ExitOnFailure(hr, "Hal is not active.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsLunarPlusActive()
{
	HRESULT hr = S_OK;
	DWORD dwCurrentDolphinProduct = 0;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");

	hr = GetActualDolphinProduct(&dwCurrentDolphinProduct);
	ExitOnSpecificValue(dwCurrentDolphinProduct, 0, hr, S_FALSE, "A dolphin product is not running.");

	hr = IsSpecificDolphinProductActive(dwCurrentDolphinProduct, DOLACCESS_LUNARPLUS);
	ExitOnFailure(hr, "LunarPlus is not active.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsSuperNovaActive()
{
	HRESULT hr = S_OK;
	DWORD dwCurrentDolphinProduct = 0;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");

	hr = GetActualDolphinProduct(&dwCurrentDolphinProduct);
	ExitOnSpecificValue(dwCurrentDolphinProduct, 0, hr, S_FALSE, "A dolphin product is not running.");

	hr = IsSpecificDolphinProductActive(dwCurrentDolphinProduct, DOLACCESS_SUPERNOVA);
	ExitOnFailure(hr, "SuperNova is not active.");
	LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsSpecificDolphinProductActive(DWORD dwCurrentDolphinProduct, DWORD dwExpectedDolphinProduct)
{
	HRESULT hr = S_OK;
		ExitIfValueNotEqualToSuppliedValue(dwCurrentDolphinProduct, dwExpectedDolphinProduct, hr, S_FALSE, "The expect dolphin product is not active.");
	LExit:
	return hr;
}