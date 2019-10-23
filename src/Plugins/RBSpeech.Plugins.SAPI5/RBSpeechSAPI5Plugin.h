#ifndef RBSPEECH_SAPI5_PLUGIN_H
#define RBSPEECH_SAPI5_PLUGIN_H
#pragma once
#include "pch.h"
#include "RBSpeechPlugin.h"
//C++ include files.
#include <memory>

//Windows header files.
#include <atlbase.h>
#include <sapi51.h>

namespace RaisedBar {
	namespace RBSpeech {
		namespace Plugins {
			class CRBSpeechSAPI5Plugin : public CRBSpeechPlugin
			{
			public:
				HRESULT IsPluginForAnAssistiveTechnology();
				HRESULT IsProductActive();
				HRESULT Silence();
				HRESULT SpeakText(BSTR text, VARIANT_BOOL silence);
				HRESULT BrailleText(BSTR text);
				std::optional<std::wstring> GetAssistiveTechnologyExecutable();
				//Factory method.
				static std::shared_ptr<CRBSpeechSAPI5Plugin> create() {
					return std::make_shared<CRBSpeechSAPI5Plugin>();
				}

			private:
				HRESULT IsAPILoaded();
				HRESULT LoadAPI();
				HRESULT UnloadAPI();

				//private fields.
			private:
						bool isAPILoaded = false;
						CComPtr<ISpVoice> sapiVoice =nullptr;
			};
		}
	}
}
#endif