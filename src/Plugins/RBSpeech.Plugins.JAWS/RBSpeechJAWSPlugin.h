#ifndef RBSPEECH_JAWS_PLUGIN_H
#define RBSPEECH_JAWS_PLUGIN_H
#pragma once
#include "stdafx.h"
#include "RBSpeechPlugin.h"

//Windows includes:
#include <Ole2.h>

//Standard C++ includes.
#include <memory>

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
				//Factory method.
				static std::shared_ptr<CRBSpeechJAWSPlugin> create() {
					return std::make_shared<CRBSpeechJAWSPlugin>();
				}

			private:
				HRESULT IsAPILoaded();
				HRESULT LoadAPI();
				HRESULT UnloadAPI();
				//private fields.
			private:

				bool isAPILoaded = false;
			};

			BOOST_DLL_ALIAS(
				RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::create, // <-- this function is exported with...
				create_plugin                               // <-- ...this alias name
			)

		}
	}
}

#endif // !RBSPEECH_NVDA_PLUGIN_H