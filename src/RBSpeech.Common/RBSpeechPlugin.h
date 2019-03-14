#ifndef RBSPEECH_PLUGIN_H
#define RBSPEECH_PLUGIN_H
#pragma once
#include "stdafx.h"

class CRBSpeechPlugin
{
public:
	virtual HRESULT IsPluginForAnAssistiveTechnology() = 0;
	virtual HRESULT IsProductActive() = 0;
	virtual HRESULT Silence() = 0;
	virtual HRESULT SpeakText(BSTR text, VARIANT_BOOL silence) = 0;
	virtual HRESULT BrailleText(BSTR text) = 0;
protected:
	inline HRESULT CheckAndLoadAPI()
	{
		HRESULT hr = S_FALSE;
		hr = IsAPILoaded();
		ExitOnSuccess(hr, "The requested API is already loaded.");
		hr = LoadAPI();
		ExitOnFailure(hr, "The requested API could not be loaded.");
	LExit:
		return hr;
	}

private:
	virtual HRESULT IsAPILoaded() = 0;
	virtual HRESULT LoadAPI() = 0;
	virtual HRESULT UnloadAPI() = 0;
};
#endif // !RBSPEECH_PLUGIN_H