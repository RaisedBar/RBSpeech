#include "stdafx.h"
#include "RBSpeechJAWSPlugin.h"
#include "OSFunctions.h"

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
	hr = IsWindowAvailable(L"JFWUI2", L"");
	ExitOnFailure(hr, "JAWS is not active.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::Silence()
{
	HRESULT hr = S_OK;
	CComVariant vFunctionResult;
	DISPID dispID = -1;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The JAWS API could not be loaded.");
	hr = FindDispIDForMethodOrPropertyName(L"StopString", &dispID);
	ExitOnFailure(hr, "The dispatch ID could not be found for the JAWS StopString function.");
	//Call jaws.
	hr = JawsAPI.Invoke0(dispID, &vFunctionResult);
	ExitOnFailure(hr, "Executing the JAWS StopString function returned a failure.");
	ExitIfValueNotEqualToSuppliedValue(vFunctionResult.vt, VT_BOOL, hr, S_FALSE, "The JAWS StopString function should return a boolean.");
	ExitOnFalse(vFunctionResult.boolVal, hr, S_FALSE, "The JAWS StopString function could not terminate any existing speech.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::SpeakText(BSTR text, VARIANT_BOOL silence)
{
	HRESULT hr = S_OK;
	CComVariant vText;
	CComVariant vSilence = silence;
	CComVariant vFunctionResult;
	DISPID dispID = -1;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be spoken was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "No text has been specified.");
	vText = text;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The JAWS API could not be loaded.");
	hr = FindDispIDForMethodOrPropertyName(L"SayString", &dispID);
	ExitOnFailure(hr, "The dispatch ID could not be found for the JAWS SayString function.");
	hr = JawsAPI.Invoke2(dispID, &vText, &vSilence, &vFunctionResult);
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
	CComVariant vBrailleTextFunctionParameter;
	DISPID dispID = -1;
	LPWSTR szBrailleMessageFunctionFormatString = nullptr;
	LPWSTR szFormattedBrailleMessageFunction = nullptr;
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be brailled was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "No text has been specified to be brailled.");
	hr =StrAllocStringSecure(&szBrailleMessageFunctionFormatString, L"BrailleMessage(\"%s\", 2000)", 0);
	ExitOnFailure(hr, S_FALSE, "Unable to create the braille message function format string.");
	hr = StrAllocFormattedSecure(&szFormattedBrailleMessageFunction, szBrailleMessageFunctionFormatString, text);
	ExitOnFailure(hr, S_FALSE, "Unable to format the full braille message function.");
	vBrailleTextFunctionParameter = szFormattedBrailleMessageFunction;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The JAWS API could not be loaded.");
	hr = FindDispIDForMethodOrPropertyName(L"RunFunction", &dispID);
	ExitOnFailure(hr, "The dispatch ID could not be found for the JAWS SayString function.");
	hr = JawsAPI.Invoke1(dispID, &vBrailleTextFunctionParameter, &vFunctionResult);
	ExitOnFailure(hr, "Executing the JAWS RunFunction function returned a failure.");
	ExitIfValueNotEqualToSuppliedValue(vFunctionResult.vt, VT_BOOL, hr, S_FALSE, "The JAWS RunFunction function should return a boolean.");
	ExitOnFalse(vFunctionResult.boolVal, hr, S_FALSE, "The JAWS RunFunction function could not schedule the speech.");
	LExit:
	ReleaseNullStr(szBrailleMessageFunctionFormatString);
	ReleaseNullStr(szFormattedBrailleMessageFunction);
	return hr;
	}

std::optional<wstring> RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::GetAssistiveTechnologyExecutable()
{
	return L"jfw.exe";
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsAPILoaded()
{
	HRESULT hr = S_OK;
	ExitOnFailure(isAPILoaded, hr, S_FALSE, "The JAWS API is not loaded.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::LoadAPI()
{
	CComPtr<IDispatch> lpTDispatch =nullptr;
	CComBSTR bJawsAPIClassID = L"FreedomSci.JawsApi";
	HRESULT hr = S_OK;
	//first initialize com for the current thread.
hr =	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
ExitOnFailure(hr, "Unable to initialize COM for the current thread.");
hr = lpTDispatch.CoCreateInstance(bJawsAPIClassID);
ExitOnFailure(hr, "Unable to load the JAWS API.");
JawsAPI = lpTDispatch;
isAPILoaded = S_OK;
LExit:
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

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::FindDispIDForMethodOrPropertyName(__in_z LPCWSTR lzMethodOrPropertyName, __out DISPID* dispID)
{
	HRESULT hr = S_OK;
	DISPID dispid = -1;
	unordered_map<wstring, DISPID>::iterator existingDispID = methodNamesToDispIDs.end();
	CComBSTR BMethodOrPropertyName = nullptr;
	ExitOnNull(lzMethodOrPropertyName, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "The method or property name for which you want the DispID is null.");
	ExitOnSpecificValue(lstrlen(lzMethodOrPropertyName), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "The method or property name for which you want the DispID is an empty string.");
	existingDispID = methodNamesToDispIDs.find(lzMethodOrPropertyName);
	if (existingDispID != methodNamesToDispIDs.end())
	{
		//The method or property name already exists in the map, so just return the disp id and exit.
		dispid = existingDispID ->second;
		goto LExit;
	}
	BMethodOrPropertyName = lzMethodOrPropertyName;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The JAWS API could not be loaded.");
	hr = JawsAPI.GetIDOfName(BMethodOrPropertyName, &dispid);
ExitOnFailure(hr, "Unable to retrieve the method or property name.");
	methodNamesToDispIDs.insert({ lzMethodOrPropertyName, dispid });
	LExit:
	dispID = &dispid;
	return hr;
}