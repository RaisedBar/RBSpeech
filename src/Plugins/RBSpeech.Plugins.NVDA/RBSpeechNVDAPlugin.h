#ifndef RBSPEECH_NVDA_PLUGIN_H
#define RBSPEECH_NVDA_PLUGIN_H
#pragma once
#include "stdafx.h"
#include "RBSpeechPlugin.h"

//Windows includes:
#include <Ole2.h>

//Standard C++ includes.
#include <memory>

// Boost includes.
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS   
#include <boost/dll/shared_library.hpp>
namespace RaisedBar {
	namespace RBSpeech {
		namespace Plugins {
//typedefs.
typedef error_status_t(WINAPI *nvdaControllerTestIfRunningFunc)(void);
typedef error_status_t(WINAPI *nvdaControllerCancelSpeechFunc)(void);
typedef error_status_t(WINAPI *nvdaControllerSpeakTextFunc)(const wchar_t *text);
typedef error_status_t(WINAPI *nvdaControllerBrailleMessageFunc)(const wchar_t *message);

class CRBSpeechNVDAPlugin : public CRBSpeechPlugin
{
public:
	HRESULT IsPluginForAnAssistiveTechnology();
	HRESULT IsProductActive();
	HRESULT Silence();
	HRESULT SpeakText(BSTR text, VARIANT_BOOL silence);
	HRESULT BrailleText(BSTR text);
//Factory method.
	static std::shared_ptr<CRBSpeechNVDAPlugin> create() {
		return std::make_shared<CRBSpeechNVDAPlugin>();
	}

private:
	HRESULT IsAPILoaded();
	HRESULT LoadAPI();
	HRESULT UnloadAPI();
//private fields.
private:
	boost::dll::shared_library NVDADllApi;
	nvdaControllerTestIfRunningFunc TestIfRunning;
	nvdaControllerCancelSpeechFunc CancelSpeech;
	nvdaControllerSpeakTextFunc SpeakMessage;
	nvdaControllerBrailleMessageFunc BrailleMessage;
	bool isAPILoaded =false;
	};

BOOST_DLL_ALIAS(
	RaisedBar::RBSpeech::Plugins::CRBSpeechNVDAPlugin::create, // <-- this function is exported with...
	create_plugin                               // <-- ...this alias name
)

		}
	}
}

#endif // !RBSPEECH_NVDA_PLUGIN_H