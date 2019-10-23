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
	return E_NOTIMPL;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	return E_NOTIMPL;
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
					ExitOnFalse(isAPILoaded, hr, S_FALSE, "The SAPI5 API is not loaded.");
				LExit:
					return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::LoadAPI()
{
	HRESULT hr = S_OK;
	hr = sapiVoice.CoCreateInstance(CLSID_SpVoice);
	ExitOnFailure(hr, S_FALSE, "Unable to create the SPVoice object.");
	isAPILoaded = true;
	LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::UnloadAPI()
{
	HRESULT hr = S_OK;
	sapiVoice.Release();
	sapiVoice = nullptr;
	ExitOnNotNull(sapiVoice, hr, S_FALSE, "Unable to release the SAPI5 voice.");
	isAPILoaded = false;
	LExit:
	return hr;
}	