#include "stdafx.h"
#include "RBSpeechDolphinPlugin.h"

//WiX includes.
#include <pathutil.h>

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
	hr = CanDolphinProductSpeak();
	ExitOnFailure(hr, "The currently active Dolphin product was unable to produce speech.");

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
	HRESULT hr = S_OK;
	hr = isAPILoaded;
	ExitOnFailure(hr, "The Dolphin API is not loaded.");
LExit:
	return hr;
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
	ExitOnFalse(exists(DolphinDllFileName), hr, E_FILENOTFOUND, "The Dolphin dll file is not found.");
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
	return nullopt;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::CanDolphinProductSpeak()
{
	HRESULT hr = S_OK;
	DWORD dwCurrentDolphinProduct = 0;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");
	hr = GetActualDolphinProduct(&dwCurrentDolphinProduct);
	ExitOnFailure(hr, "The current Dolphin product could not be obtained.");
	hr = S_FALSE;
	if ((IsHalActive(dwCurrentDolphinProduct) ==S_OK) && (IsLunarPlusActive(dwCurrentDolphinProduct) ==S_OK) && (IsSuperNovaActive(dwCurrentDolphinProduct) ==S_OK))
	{
		hr = S_OK;
	}
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
HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsHalActive(DWORD currentDolphinProduct)
{
	HRESULT hr = S_OK;
	hr = IsSpecificDolphinProductActive(currentDolphinProduct, DOLACCESS_HAL);
	ExitOnFailure(hr, "Hal is not active.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsLunarPlusActive(DWORD currentDolphinProduct)
{
	HRESULT hr = S_OK;
	hr = IsSpecificDolphinProductActive(currentDolphinProduct, DOLACCESS_LUNARPLUS);
	ExitOnFailure(hr, "LunarPlus is not active.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsSuperNovaActive(DWORD currentDolphinProduct)
{
	HRESULT hr = S_OK;
	hr = IsSpecificDolphinProductActive(currentDolphinProduct, DOLACCESS_SUPERNOVA);
	ExitOnFailure(hr, "SuperNova is not active.");
	LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsSpecificDolphinProductActive(unsigned int uCurrentDolphinProduct, unsigned int uExpectedDolphinProduct)
{
	HRESULT hr = S_OK;
		ExitIfValueNotEqualToSuppliedValue(uCurrentDolphinProduct, uExpectedDolphinProduct, hr, S_FALSE, "The expect dolphin product is not active.");
	LExit:
	return hr;
}