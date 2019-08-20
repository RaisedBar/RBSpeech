#include "stdafx.h"
#include "RBSpeechJAWSPlugin.h"
#include "OSFunctions.h"

//Windows/ATL includes.
#include <comutil.h>

//WiX includes
#include <strutil.h>

using namespace RaisedBar::RBSpeech::OSFunctions;
using namespace std;

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsPluginForAnAssistiveTechnology()
{
	return S_OK;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsProductActive()
{
	HRESULT hr = S_OK;
	hr = IsWindowAvailable(L"JFWUI2", NULL);
	ExitOnFailure(hr, "JAWS is not active.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::Silence()
{
	HRESULT hr = S_OK;
	CComVariant vFunctionResult;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The JAWS API could not be loaded.");

	//Call jaws.
	hr = JawsAPI.Invoke0(_bstr_t("StopString"), &vFunctionResult);
	ExitOnFailure(hr, "Executing the JAWS StopString function returned a failure.");
	ExitIfValueNotEqualToSuppliedValue(vFunctionResult.vt, VT_BOOL, hr, S_FALSE, "The JAWS StopString function should return a boolean.");
	ExitOnFalse(vFunctionResult.boolVal, hr, S_FALSE, "The JAWS StopString function could not terminate any existing speech.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	HRESULT hr = S_OK;
	CComVariant vFunctionResult;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be spoken was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "No text has been specified.");
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The JAWS API could not be loaded.");
	hr = JawsAPI.Invoke2(_bstr_t(L"SayString"), &_variant_t(text), &_variant_t(silence), &vFunctionResult);
	ExitOnFailure(hr, "Executing the JAWS SayString function returned a failure.");
	ExitIfValueNotEqualToSuppliedValue(vFunctionResult.vt, VT_BOOL, hr, S_FALSE, "The JAWS SayString function should return a boolean.");
	ExitOnFalse(vFunctionResult.boolVal, hr, S_FALSE, "The JAWS SayString function could not schedule the speech.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::BrailleText(BSTR text)
{
	HRESULT hr = S_OK;
	CComVariant vFunctionResult;
	LPCWSTR sczBrailleMessageFunctionFormatString = L"BrailleMessage(\"%s\", 2000)";
	LPWSTR szFormattedBrailleMessageFunction = nullptr;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be brailled was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "No text has been specified to be brailled.");
	hr = StrAllocFormattedSecure(&szFormattedBrailleMessageFunction, sczBrailleMessageFunctionFormatString, text);
	ExitOnFailure(hr, S_FALSE, "Unable to formate the ");
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The JAWS API could not be loaded.");
	hr = JawsAPI.Invoke1(_bstr_t(L"RunFunction"), &_variant_t(szFormattedBrailleMessageFunction), &vFunctionResult);
	ExitOnFailure(hr, "Executing the JAWS RunFunction function returned a failure.");
	ExitIfValueNotEqualToSuppliedValue(vFunctionResult.vt, VT_BOOL, hr, S_FALSE, "The JAWS RunFunction function should return a boolean.");
	ExitOnFalse(vFunctionResult.boolVal, hr, S_FALSE, "The JAWS RunFunction function could not schedule the speech.");
	LExit:
	return hr;
	}

std::optional<wstring> RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::GetAssistiveTechnologyExecutable()
{
	return L"jfw.exe";
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsAPILoaded()
{
	HRESULT hr = S_OK;
	ExitOnFalse(isAPILoaded, hr, S_FALSE, "The NVDA API is not loaded.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::LoadAPI()
{
	CComPtr<IDispatch> lpTDispatch =nullptr;
	HRESULT hr = S_OK;
	//first initialize com for the current thread.
hr =	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
ExitOnFailure(hr, "Unable to initialize COM for the current thread.");
hr = lpTDispatch.CoCreateInstance(_bstr_t(L"FreedomSci.JawsApi"));
ExitOnFailure(hr, "Unable to load the JAWS API.");
JawsAPI = lpTDispatch;
isAPILoaded = S_OK;
LExit:
lpTDispatch.Release();
return hr;
	}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::UnloadAPI()
{
	HRESULT hr = S_OK;
	hr = IsAPILoaded();
	ExitOnFailure(hr, "The JAWS API is not loaded.");
	ExitOnNull(JawsAPI, hr, S_FALSE, "The JAWS API is null.");
	JawsAPI.Release();
	JawsAPI = nullptr;
	ExitOnNotNull(JawsAPI, hr, S_FALSE, "We were not able to release the JAWS API.");
	CoUninitialize();
LExit:
	return hr;
}