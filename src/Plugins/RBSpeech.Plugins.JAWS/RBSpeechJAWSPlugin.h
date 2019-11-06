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

#define BOOST_DLL_USE_STD_FS
//boost includes.
#include <boost/dll/alias.hpp>
#include <boost/dll/shared_library.hpp>

namespace RaisedBar {
	namespace RBSpeech {
		namespace Plugins {
			//Typedefs.
			typedef int(WINAPI* GetNumberOfJAWSVersionsInstalledFunc)(void);
			typedef int(WINAPI* GetIndexOfRunningJAWSFunc)(void);

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
				HRESULT IsJAWSSetupUtilityLoaded();
				HRESULT LoadJAWSSetupUtility();
				HRESULT UnloadJAWSSetupUtility();

				//private fields.
			private:
				CComDispatchDriver JawsAPI;
				bool isAPILoaded =false;
				DISPID sayStringDispID =-1;
				DISPID stopStringDispID =-1;
				DISPID runFunctionDispID =-1;
				boost::dll::shared_library jawsSetupUtilityDllApi;
				GetNumberOfJAWSVersionsInstalledFunc GetNumberOfJAWSVersionsInstalled = nullptr;
				GetIndexOfRunningJAWSFunc GetIndexOfRunningJAWS = nullptr;
			};

			BOOST_DLL_ALIAS(
				RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::create, // <-- this function is exported with...
				create_plugin                               // <-- ...this alias name
			)

		}
	}
}

#endif // !RBSPEECH_NVDA_PLUGIN_H