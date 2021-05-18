#ifndef RBSPEECH_DOLPHIN_PLUGIN_H
#define RBSPEECH_DOLPHIN_PLUGIN_H
#pragma once
#include "stdafx.h"
#include "RBSpeechPlugin.h"

//Standard C++ includes.
#include <memory>
#include <optional>
#include <string>
#include <vector>

#define BOOST_DLL_USE_STD_FS
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

			//return codes from functions other than DolAccess_GetSystem
#define DOLACCESS_SUCCESS						0
#define DOLACCESS_FAIL_NO_SERVER				1
#define DOLACCESS_FAIL_INVALID_ID				2
#define DOLACCESS_FAIL_BAD_LEN					3
#define DOLACCESS_FAIL_INVALID_ACTION			4
#define DOLACCESS_FAIL_NOT_SUPPORTED_BY_SERVER	5
#define DOLACCESS_FAIL_BAD_STRING				6
#define DOLACCESS_FAIL_TIMED_OUT				7
#define DOLACCESS_FAIL_READ_ONLY				8
#define DOLACCESS_FAIL_SERVER_BUSY				9

			// id's for DolAccess_Command 
#define DOLAPI_COMMAND_SPEAK 1
#define DOLAPI_COMMAND_LOAD_SCRIPT 2
#define DOLAPI_COMMAND_UNLOAD_SCRIPT 3
#define DOLAPI_COMMAND_SCRIPT_FUNCTION 4
//Defines for Dolphin actions.
#define DOLACTION_SILENCE 141
			class CRBSpeechDolphinPlugin : public CRBSpeechPlugin
			{
			public:
				HRESULT IsPluginForAnAssistiveTechnology();
				HRESULT IsProductActive();
				HRESULT Silence();
				HRESULT SpeakText(BSTR text, VARIANT_BOOL silence);
				HRESULT BrailleText(BSTR text);
				std::optional<std::wstring> GetAssistiveTechnologyExecutable();
				//Factory method.
				static std::shared_ptr<CRBSpeechDolphinPlugin> create() {
					return std::make_shared<CRBSpeechDolphinPlugin>();
				}

			private:
				inline HRESULT IsAPILoaded();
				HRESULT LoadAPI();
				HRESULT UnloadAPI();
				HRESULT CanDolphinProductSpeak();
				HRESULT GetActualDolphinProduct(__out DWORD *pdwActualProduct);
				HRESULT IsHalActive();
				HRESULT IsLunarPlusActive();
				HRESULT IsSuperNovaActive();
				HRESULT IsSpecificDolphinProductActive(DWORD DWCurrentDolphinProduct, DWORD dwExpectedDolphinProduct);

				//private fields.
			private:
				boost::dll::shared_library DolphinDllApi;
				DolAccess_GetSystemFunc DolAccess_GetSystem =nullptr;
				DolAccess_ActionFunc DolAccess_Action =nullptr;
				DolAccess_CommandFunc DolAccess_Command =nullptr;
				HRESULT isAPILoaded = S_FALSE;
				std::vector< LPCWSTR> dolphinProcessNames = {L"snova.exe"};
			};
			BOOST_DLL_ALIAS(
				RaisedBar::RBSpeech::Plugins::CRBSpeechDolphinPlugin::create, // <-- this function is exported with...
				create_plugin                               // <-- ...this alias name
			)


			}
	}
}

#endif // !RBSPEECH_NVDA_PLUGIN_H