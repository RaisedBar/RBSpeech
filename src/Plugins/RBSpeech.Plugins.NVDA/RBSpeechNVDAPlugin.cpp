#include "stdafx.h"
#include "RBSpeechNVDAPlugin.h"

HRESULT CRBSpeechNVDAPlugin::IsPluginForAnAssistiveTechnology()
{
	return S_FALSE;
}

HRESULT CRBSpeechNVDAPlugin::IsProductActive()
{
	return S_FALSE;
}

HRESULT CRBSpeechNVDAPlugin::Silence()
{
	return S_FALSE;
}

HRESULT CRBSpeechNVDAPlugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	return S_FALSE;
}

HRESULT CRBSpeechNVDAPlugin::BrailleText(BSTR text)
{
	return S_FALSE;
}
	
HRESULT CRBSpeechNVDAPlugin::IsAPILoaded()
{
	return S_FALSE;
}
	
HRESULT CRBSpeechNVDAPlugin::LoadAPI()
{
	return S_FALSE;
}
	
HRESULT CRBSpeechNVDAPlugin::UnloadAPI()
{
	return S_FALSE;
}