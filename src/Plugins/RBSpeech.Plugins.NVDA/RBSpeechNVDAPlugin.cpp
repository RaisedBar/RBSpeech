#include "stdafx.h"
#include "RBSpeechNVDAPlugin.h"

//Wix includes:
#include <pathutil.h>

//Standard C++ includes.
#include <filesystem>
using namespace std::experimental::filesystem;

//Boost includes.
#include <boost/dll/import.hpp>

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::IsPluginForAnAssistiveTechnology()
{
	return S_OK;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::IsProductActive()
{
	HRESULT hr = S_OK;
	hr = IsAPILoaded();
	if (hr == S_FALSE)
	{
		hr = LoadAPI();
		ExitOnFailure(hr, "The NVDA API could not be loaded.");
}
	
	ExitIfValueNotEqualToSuppliedValue(TestIfRunning(), 0, hr, S_FALSE, "NVDA is not running.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::Silence()
{
	HRESULT hr = S_OK;
	hr = IsAPILoaded();
	if (hr == S_FALSE)
	{
		hr = LoadAPI();
		ExitOnFailure(hr, "The NVDA API could not be loaded.");
	}

	ExitIfValueNotEqualToSuppliedValue(CancelSpeech(), 0, hr, S_FALSE, "Silencing NVDA failed.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	HRESULT hr = S_OK;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be spoken was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "The message to be spoken was an empty string.");

	hr = IsAPILoaded();
	if (hr == S_FALSE)
	{
		hr = LoadAPI();
		ExitOnFailure(hr, "The NVDA API could not be loaded.");
	}
	
	if (silence == VARIANT_TRUE)
	{
		//Silence any existing speech.
		hr = Silence();
		ExitOnFailure(hr, "Unable to silence existing speech.");
}
	
	ExitIfValueNotEqualToSuppliedValue(SpeakMessage(text), 0, hr, S_FALSE, "Speaking through NVDA failed.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::BrailleText(BSTR text)
{
	HRESULT hr = S_OK;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be brailled was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "The message to be brailled was an empty string.");

	hr = IsAPILoaded();
	if (hr == S_FALSE)
	{
		hr = LoadAPI();
		ExitOnFailure(hr, "The NVDA API could not be loaded.");
	}

	ExitIfValueNotEqualToSuppliedValue(BrailleMessage(text), 0, hr, S_FALSE, "brailling through NVDA failed.");
LExit:
	return hr;
}
	
HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::IsAPILoaded()
{
	HRESULT hr = S_OK;
	ExitOnFalse(isAPILoaded, hr, S_FALSE, "The NVDA API is not loaded.");
LExit:
	return hr;
}
	
HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::LoadAPI()
{
	HRESULT hr = S_OK;
	LPWSTR sczCurrentPath = NULL;
	path NVDADllFileName ;
	//Change the following line once we have an ExitOnSuccess macro.
	ExitOnSpecificValue(IsAPILoaded(), S_OK, hr, S_OK, "The NVDA API is already loaded.");
	hr = PathForCurrentProcess(&sczCurrentPath, NULL);
	ExitOnFailure(hr, "Failed to get current process path.");
	NVDADllFileName = sczCurrentPath;
	if (NVDADllFileName.has_filename())
	{
		NVDADllFileName.remove_filename();
	}

	//Add the nvda controler file name for 32-bit.
	NVDADllFileName /= L"nvdaControllerClient32.dll";
//Check file existence.
	ExitOnFalse(exists(NVDADllFileName), hr, E_FILENOTFOUND, "The NVDA dll file is not found.");
	//The dll file exists, so try to load it.
	NvdaDllApi.load(NVDADllFileName.generic_wstring());
	ExitOnFalse(NvdaDllApi.is_loaded(), hr, S_FALSE, "The nvda dll file could not be loaded.");
//Load the functions we need.
	ExitOnFalse(NvdaDllApi.has("nvdaController_testIfRunning"), hr, S_FALSE, "The NVDA dll does not export the nvdaController_testIfRunning function.");
	TestIfRunning = NvdaDllApi.get<nvdaControllerTestIfRunningFunc>("nvdaController_testIfRunning");
	ExitOnFalse(NvdaDllApi.has("nvdaController_speakText"), hr, S_FALSE, "The NVDA dll does not export the nvdaController_speakText function.");
	SpeakMessage = NvdaDllApi.get<nvdaControllerSpeakTextFunc>("nvdaController_speakText");
	ExitOnFalse(NvdaDllApi.has("nvdaController_brailleMessage"), hr, S_FALSE, "The NVDA dll does not export the nvdaController_brailleMessage function.");	
	BrailleMessage = NvdaDllApi.get<nvdaControllerBrailleMessageFunc>("nvdaController_brailleMessage");
	ExitOnFalse(NvdaDllApi.has("nvdaController_cancelSpeech"), hr, S_FALSE, "The NVDA dll does not export the nvdaController_cancelSpeech function.");
	CancelSpeech = NvdaDllApi.get<nvdaControllerCancelSpeechFunc>("nvdaController_cancelSpeech");
	isAPILoaded = true;
LExit:
	return hr;
}
	
HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::UnloadAPI()
{
	HRESULT hr = S_OK;
	hr = IsAPILoaded();
	ExitOnFailure(hr, "The NVDA API is not loaded.");
ExitOnNull(CancelSpeech, hr, S_FALSE, "The cancel speech function is already null.");
CancelSpeech = nullptr;
ExitOnNotNull(CancelSpeech, hr, S_FALSE, "The cancel speech function could not be released.");
ExitOnNull(BrailleMessage, hr, S_FALSE, "The braille message function is already null.");
BrailleMessage = nullptr;
ExitOnNotNull(BrailleMessage, hr, S_FALSE, "The braille message function could not be released.");
ExitOnNull(SpeakMessage, hr, S_FALSE, "The speak message function is already null.");
SpeakMessage = nullptr;
ExitOnNotNull(SpeakMessage, hr, S_FALSE, "The speak message function could not be released.");
ExitOnNull(TestIfRunning, hr, S_FALSE, "The test if running function is already null.");
TestIfRunning = nullptr;
ExitOnNotNull(TestIfRunning, hr, S_FALSE, "The test if running function could not be released.");

	LExit:
	return hr;
}
