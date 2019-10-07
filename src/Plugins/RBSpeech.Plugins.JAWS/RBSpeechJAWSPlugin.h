#ifndef RBSPEECH_JAWS_PLUGIN_H
#define RBSPEECH_JAWS_PLUGIN_H
#pragma once
#include "stdafx.h"
#include "RBSpeechPlugin.h"

//Windows includes:
#include <atlbase.h>

//Standard C++ includes.
#include <memory>
#include <string>
#include <optional>
#include <unordered_map>

#define BOOST_DLL_USE_STD_FS
//boost includes.
#include <boost/dll/alias.hpp>

namespace RaisedBar {
	namespace RBSpeech {
		namespace Plugins {
			class CRBSpeechJAWSPlugin : public CRBSpeechPlugin
			{
			public:
				HRESULT IsPluginForAnAssistiveTechnology();
				HRESULT IsProductActive();
				HRESULT Silence();
				HRESULT SpeakText(BSTR text, VARIANT_BOOL silence);
				HRESULT BrailleText(BSTR text);
				std::optional<std::wstring> GetAssistiveTechnologyExecutable();
				//Factory method.
				static std::shared_ptr<CRBSpeechJAWSPlugin> create() {
					return std::make_shared<CRBSpeechJAWSPlugin>();
				}

			private:
				HRESULT IsAPILoaded();
				HRESULT LoadAPI();
				HRESULT UnloadAPI();
				HRESULT FindDispIDForMethodOrPropertyName(__in_z LPCWSTR lzMethodOrPropertyName, __out DISPID* dispID);
				
				//private fields.
			private:
				CComDispatchDriver JawsAPI;
				HRESULT isAPILoaded = S_FALSE;
				std::unordered_map<std::wstring, DISPID> methodNamesToDispIDs;
			};

			BOOST_DLL_ALIAS(
				RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::create, // <-- this function is exported with...
				create_plugin                               // <-- ...this alias name
			)

		}
	}
}

#endif // !RBSPEECH_NVDA_PLUGIN_H