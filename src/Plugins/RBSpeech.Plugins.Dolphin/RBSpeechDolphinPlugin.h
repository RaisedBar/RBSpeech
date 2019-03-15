#ifndef RBSPEECH_DOLPHIN_PLUGIN_H
#define RBSPEECH_DOLPHIN_PLUGIN_H
#pragma once
#include "stdafx.h"
#include "RBSpeechPlugin.h"
 
//Standard C++ includes.
#include <memory>

//boost includes.
#include <boost/dll/alias.hpp>
#include <boost/dll/shared_library.hpp>

namespace RaisedBar {
	namespace RBSpeech {
		namespace Plugins {
			//Typedefs.
			//typedefs.
			typedef DWORD (WINAPI *DolAccess_GetSystemFunc)(void);
			typedef DWORD (WINAPI *DolAccess_ActionFunc)(DWORD Id);
			typedef DWORD(WINAPI *DolAccess_CommandFunc)(VOID *Par, DWORD Len, DWORD Id);
			
			// return codes from DolAccess_GetSystem
#define DOLACCESS_NONE 0
#define DOLACCESS_HAL 1
#define DOLACCESS_LUNAR 2
#define DOLACCESS_SUPERNOVA 4
#define DOLACCESS_LUNARPLUS 8

			class CRBSpeechDolphinPlugin : public CRBSpeechPlugin
			{
			public:
				HRESULT IsPluginForAnAssistiveTechnology();
				HRESULT IsProductActive();
				HRESULT Silence();
				HRESULT SpeakText(BSTR text, VARIANT_BOOL silence);
				HRESULT BrailleText(BSTR text);
				//Factory method.
				static std::shared_ptr<CRBSpeechDolphinPlugin> create() {
					return std::make_shared<CRBSpeechDolphinPlugin>();
				}

			private:
				HRESULT IsAPILoaded();
				HRESULT LoadAPI();
				HRESULT UnloadAPI();
				HRESULT CanDolphinProductSpeak();
				HRESULT GetActualDolphinProduct(__out DWORD *pdwActualProduct);

				//private fields.
			private:
				boost::dll::shared_library DolphinDllApi;
				DolAccess_GetSystemFunc DolAccess_GetSystem;
				DolAccess_ActionFunc DolAccess_Action;
				DolAccess_CommandFunc DolAccess_Command;
				HRESULT isAPILoaded = S_FALSE;
			};

			BOOST_DLL_ALIAS(
				RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::create, // <-- this function is exported with...
				create_plugin                               // <-- ...this alias name
			)

		}
	}
}

#endif // !RBSPEECH_NVDA_PLUGIN_H