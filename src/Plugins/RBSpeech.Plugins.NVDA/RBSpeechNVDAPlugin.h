#ifndef RBSPEECH_NVDA_PLUGIN_H
#define RBSPEECH_NVDA_PLUGIN_H
#pragma once
#include "stdafx.h"
#include "RBSpeechPlugin.h"
#include <Ole2.h>

class CRBSpeechNVDAPlugin : CRBSpeechPlugin
{
public:
	HRESULT IsPluginForAnAssistiveTechnology();
	HRESULT IsProductActive();
	HRESULT Silence();
	HRESULT SpeakText(BSTR text, VARIANT_BOOL silence);
	HRESULT BrailleText(BSTR text);

private:
	HRESULT IsAPILoaded();
	HRESULT LoadAPI();
	HRESULT UnloadAPI();
};
#endif // !RBSPEECH_NVDA_PLUGIN_H


