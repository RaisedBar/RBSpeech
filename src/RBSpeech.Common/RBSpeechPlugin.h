#ifndef RBSPEECH_PLUGIN_H
#define RBSPEECH_PLUGIN_H
#pragma once
#include "stdafx.h"

public class CRBSpeechPlugin
{
public:
	virtual HRESULT IsPluginForAnAssistiveTechnology() = 0;
	virtual HRESULT IsProductActive() = 0;
	virtual HRESULT Silence() = 0;
	virtual HRESULT SpeakText(BSTR text, VARIANT_BOOL silence) = 0;
	virtual HRESULT BrailleText(BSTR text) = 0;
};
#endif // !RBSPEECH_PLUGIN_H