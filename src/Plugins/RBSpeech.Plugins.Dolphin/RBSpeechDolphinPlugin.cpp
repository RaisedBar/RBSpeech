#include "stdafx.h"
#include "RBSpeechDolphinPlugin.h"

//WiX includes.
#include <pathutil.h>

//C++ standard includes.
#include <filesystem>
using namespace std::experimental::filesystem;

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsPluginForAnAssistiveTechnology()
{
	return S_OK;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::IsProductActive()
{
	HRESULT hr = S_OK;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The Dolphin API could not be loaded.");
	currentDolphinProduct = DolAccess_GetSystem();
	ExitOnSpecificValue(currentDolphinProduct, 0, hr, S_FALSE, "A dolphin product is not running.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::Silence()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::BrailleText(BSTR text)
{
	HRESULT hr =E_NOTIMPL;
	LExit:
	return hr;
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
	isAPILoaded = S_FALSE;
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