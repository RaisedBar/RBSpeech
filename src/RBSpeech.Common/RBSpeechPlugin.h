#ifndef RBSPEECH_PLUGIN_H
#define RBSPEECH_PLUGIN_H
#pragma once
#include "stdafx.h"
#include "WiXMacros.h"
#include <string>
#include <optional>

class CRBSpeechPlugin
{
public:
	virtual HRESULT IsPluginForAnAssistiveTechnology() = 0;
	virtual HRESULT IsProductActive() = 0;
	virtual HRESULT Silence() = 0;
	virtual HRESULT SpeakText(BSTR text, VARIANT_BOOL silence) = 0;
	virtual HRESULT BrailleText(BSTR text) = 0;
	virtual std::optional<std::wstring> GetAssistiveTechnologyExecutable() = 0;
protected:
	HRESULT CheckAndLoadAPI();
	
private:
	virtual HRESULT IsAPILoaded() = 0;
	virtual HRESULT LoadAPI() = 0;
	virtual HRESULT UnloadAPI() = 0;
};
#endif // !RBSPEECH_PLUGIN_H