#include "pch.h"
#include "RBSpeechSAPI5Plugin.h"
//WiX includes.
#include <regutil.h>

using namespace std;

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::IsPluginForAnAssistiveTechnology()
{
	return S_FALSE;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::IsProductActive()
{
	HRESULT hr = S_OK;
	HKEY hOpenedKey = nullptr;
	hr = RegInitialize();
	ExitOnFailure(hr , S_FALSE, "Unable to initialize the WiX registry functions.");
	hr = RegOpen(HKEY_CLASSES_ROOT, L"SAPI.SpVoice", KEY_READ, &hOpenedKey);
	ExitOnFailure(hr, E_FILENOTFOUND, "SAPI5 is not installed.");
	LExit:
	ReleaseRegKey(hOpenedKey);
	RegUninitialize();
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::Silence()
{
	HRESULT hr = S_OK;
	SpeechVoiceSpeakFlags flags = SpeechVoiceSpeakFlags::SVSFPurgeBeforeSpeak;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The SAPI 5 API could not be loaded.");
	hr = sapiVoice->Speak(NULL, flags, 0);
	ExitOnFailure(hr, "Unable to silence speech.");
	LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	HRESULT hr = S_OK;
	SpeechVoiceSpeakFlags flags = SpeechVoiceSpeakFlags::SVSFDefault;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be spoken was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "No text has been specified.");

	if (silence == VARIANT_TRUE)
	{
		hr = Silence();
		ExitOnFailure(hr, "Unable to silence speech.");
		}

	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The SAPI 5 API could not be loaded.");
	
	hr = sapiVoice->Speak(text, flags, NULL);
	ExitOnFailure(hr, "Executing the SAPI5 Speak function returned an error.");
	LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::BrailleText(BSTR text)
{
	return E_NOTIMPL;
}

optional<wstring> RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::GetAssistiveTechnologyExecutable()
{
	return nullopt;
}

				HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::IsAPILoaded()
{
					HRESULT hr = S_OK;
					ExitOnTrue(!sapiVoice, hr, S_FALSE, "The SAPI5 API is not loaded.");
				LExit:
					return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::LoadAPI()
{
	HRESULT hr = S_OK;
	hr = sapiVoice.CoCreateInstance(CLSID_SpVoice);
	ExitOnFailure(hr, S_FALSE, "Unable to create the SPVoice object.");
	LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::UnloadAPI()
{
	HRESULT hr = S_OK;
	hr = IsAPILoaded();
	ExitOnFailure(hr, "The SAPI 5 API is not loaded.");
	sapiVoice.Release();
	sapiVoice = nullptr;
	ExitOnFalse(!sapiVoice, hr, S_FALSE, "We were not able to release the SAPI5 voice.");
	LExit:
	return hr;
}	