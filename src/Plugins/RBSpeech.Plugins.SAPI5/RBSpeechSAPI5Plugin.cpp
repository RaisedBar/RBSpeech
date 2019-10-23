#include "pch.h"
#include "RBSpeechSAPI5Plugin.h"
using namespace std;

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::IsPluginForAnAssistiveTechnology()
{
	return S_FALSE;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::IsProductActive()
{
	return E_NOTIMPL;
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
					return E_NOTIMPL;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::LoadAPI()
{
	return E_NOTIMPL;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechSAPI5Plugin::UnloadAPI()
{
	return E_NOTIMPL;
}
