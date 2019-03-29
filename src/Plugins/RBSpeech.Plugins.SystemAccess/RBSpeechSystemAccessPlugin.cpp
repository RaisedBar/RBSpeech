#include "stdafx.h"
#include "RBSpeechSystemAccessPlugin.h"

//Wix includes:
#include <pathutil.h>

//Standard C++ includes.
#include <filesystem>
using namespace std;
using namespace std::filesystem;

//Boost includes.
#include <boost/dll/import.hpp>

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::IsPluginForAnAssistiveTechnology()
{
	return S_OK;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::IsProductActive()
{
	HRESULT hr = S_OK;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The NVDA API could not e loaded.");

	ExitOnFalse(SAIsRunning(), hr, S_FALSE, "System Access is not running.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::Silence()
{
	HRESULT hr = S_OK;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The NVDA API could not e loaded.");

	ExitOnFalse(SAStopAudio(), hr, S_FALSE, "Silencing System Access failed.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	HRESULT hr = S_OK;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be spoken was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "The message to be spoken was an empty string.");

	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The NVDA API could not e loaded.");

	if (silence == VARIANT_TRUE)
	{
		//Silence any existing speech.
		hr = Silence();
		ExitOnFailure(hr, "Unable to silence existing speech.");
	}

	ExitOnFalse(SASpeak(text), hr, S_FALSE, "Speaking through System Access failed.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::BrailleText(BSTR text)
{
	HRESULT hr = S_OK;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be brailled was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "The message to be brailled was an empty string.");

	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The NVDA API could not e loaded.");

	ExitOnFalse(SABraille, hr, S_FALSE, "brailling through System Access failed.");
LExit:
	return hr;
}

optional<wstring> RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::GetAssistiveTechnologyExecutable()
{
	return L"sa.exe";
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::IsAPILoaded()
{
	HRESULT hr = S_OK;
	ExitOnFalse(isAPILoaded, hr, S_FALSE, "The NVDA API is not loaded.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::LoadAPI()
{
	HRESULT hr = S_OK;
	LPWSTR sczCurrentPath = NULL;
	path SystemAccessDllFileName;
	hr = IsAPILoaded();
	ExitOnSuccess(hr, "The System Access API is already loaded.");
	hr = PathForCurrentProcess(&sczCurrentPath, NULL);
	ExitOnFailure(hr, "Failed to get current process path.");
	SystemAccessDllFileName = sczCurrentPath;
	if (SystemAccessDllFileName.has_filename())
	{
		SystemAccessDllFileName.remove_filename();
	}

	//Add the System Access dll file name for 32-bit.
	SystemAccessDllFileName /= L"SAAPI32.dll";
	//Check file existence.
	ExitOnFalse(exists(SystemAccessDllFileName), hr, E_FILENOTFOUND, "The System Access dll file is not found.");
	//The dll file exists, so try to load it.
	SystemAccessDllApi.load(SystemAccessDllFileName.generic_wstring());
	ExitOnFalse(SystemAccessDllApi.is_loaded(), hr, S_FALSE, "The System Access dll file could not be loaded.");
	//Load the functions we need.
	ExitOnFalse(SystemAccessDllApi.has("SA_IsRunning"), hr, S_FALSE, "The System Access dll does not export the SA_IsRunning function.");
	SAIsRunning = SystemAccessDllApi.get<SAIsRunningFunc>("SA_IsRunning");
	ExitOnFalse(SystemAccessDllApi.has("SA_SayW"), hr, S_FALSE, "The System Access dll does not export the SA_SayW function.");
	SASpeak = SystemAccessDllApi.get<SASpeakFunc>("SA_SayW");
	ExitOnFalse(SystemAccessDllApi.has("SA_BrlShowTextW"), hr, S_FALSE, "The System Access dll does not export the SA_BrlShowTextW function.");
	SABraille = SystemAccessDllApi.get<SABrailleFunc>("SA_BrlShowTextW");
	ExitOnFalse(SystemAccessDllApi.has("SA_StopAudio"), hr, S_FALSE, "The System Access dll does not export the SA_StopAudio function.");
	SAStopAudio = SystemAccessDllApi.get<SAStopAudioFunc>("SA_StopAudio");
	isAPILoaded = true;
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::UnloadAPI()
{
	HRESULT hr = S_OK;
	hr = IsAPILoaded();
	ExitOnFailure(hr, "The System Access API is not loaded.");
	ExitOnNull(SAStopAudio, hr, S_FALSE, "The SAStopAudio function is already null.");
	SAStopAudio = nullptr;
	ExitOnNotNull(SAStopAudio, hr, S_FALSE, "The SAStopAudio function could not be released.");
	ExitOnNull(SABraille, hr, S_FALSE, "The SABraille function is already null.");
	SABraille = nullptr;
	ExitOnNotNull(SABraille, hr, S_FALSE, "The SABraille function could not be released.");
	ExitOnNull(SASpeak, hr, S_FALSE, "The SASpeak function is already null.");
	SASpeak = nullptr;
	ExitOnNotNull(SASpeak, hr, S_FALSE, "The SASpeak function could not be released.");
	ExitOnNull(SAIsRunning, hr, S_FALSE, "The SAIsRunning function is already null.");
	SAIsRunning = nullptr;
	ExitOnNotNull(SAIsRunning, hr, S_FALSE, "The SAIsRunning function could not be released.");
	ExitOnFalse(SystemAccessDllApi.is_loaded(), hr, S_FALSE, "The System Access dll file is not loaded.");
	SystemAccessDllApi.unload();
	ExitOnTrue(SystemAccessDllApi.is_loaded(), hr, S_FALSE, "The System Access dll is still loaded.");
	isAPILoaded = false;
LExit:
	return hr;
}