#ifndef RBSPEECH_SYSTEM_ACCESS_PLUGIN_H
#define RBSPEECH_SYSTEM_ACCESS_PLUGIN_H
#pragma once
#include "stdafx.h"
#include "RBSpeechPlugin.h"

//Standard C++ includes.
#include <memory>
#include <optional>
#include <string>

#define BOOST_DLL_USE_STD_FS
// Boost includes.
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS   
#include <boost/dll/shared_library.hpp>

namespace RaisedBar {
	namespace RBSpeech {
		namespace Plugins {
			//typedefs.
			typedef BOOL(WINAPI *SAIsRunningFunc)(void);
			typedef BOOL(WINAPI *SABrailleFunc)(const WCHAR *textW);
			typedef BOOL(WINAPI *SASpeakFunc)(const WCHAR *textW);
			typedef BOOL(WINAPI *SAStopAudioFunc)(void);

			class CRBSpeechSystemAccessPlugin : public CRBSpeechPlugin
			{
			public:
				HRESULT IsPluginForAnAssistiveTechnology();
				HRESULT IsProductActive();
				HRESULT Silence();
				HRESULT SpeakText(BSTR text, VARIANT_BOOL silence);
				HRESULT BrailleText(BSTR text);
				std::optional<std::wstring> GetAssistiveTechnologyExecutable();
				//Factory method.
				static std::shared_ptr<CRBSpeechSystemAccessPlugin> create() {
					return std::make_shared<CRBSpeechSystemAccessPlugin>();
				}

			private:
				inline HRESULT IsAPILoaded();
				HRESULT LoadAPI();
				HRESULT UnloadAPI();

				//private fields.
			private:
				boost::dll::shared_library SystemAccessDllApi;
				SAIsRunningFunc SAIsRunning;
				SASpeakFunc SASpeak;
				SABrailleFunc SABraille;
				SAStopAudioFunc SAStopAudio;
				bool isAPILoaded = false;
			};

			BOOST_DLL_ALIAS(
				RaisedBar::RBSpeech::Plugins::CRBSpeechSystemAccessPlugin::create, // <-- this function is exported with...
				create_plugin                               // <-- ...this alias name
			)

		}
	}
}

#endif // !RBSPEECH_NVDA_PLUGIN_H