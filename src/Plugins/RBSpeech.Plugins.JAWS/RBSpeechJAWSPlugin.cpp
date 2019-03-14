#include "stdafx.h"
#include "RBSpeechJAWSPlugin.h"

//Wix includes:

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsPluginForAnAssistiveTechnology()
{
	return S_OK;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsProductActive()
{
	return S_FALSE;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::Silence()
{
	return S_FALSE;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	return S_FALSE;
	}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::BrailleText(BSTR text)
{
	return S_FALSE;
	}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsAPILoaded()
{
	return S_FALSE;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::LoadAPI()
{
	return S_FALSE;
	}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::UnloadAPI()
{
	return S_FALSE;
}