#include "stdafx.h"
#include "RBSpeechJAWSPlugin.h"

//WiX includes
#include <fileutil.h>
#include <pathutil.h>
#include <strutil.h>

using namespace std;

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsPluginForAnAssistiveTechnology()
{
	return S_OK;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsProductActive()
{
	HRESULT hr = S_OK;
	hr = LoadJAWSSetupUtility();
	ExitOnFailure(hr, S_FALSE, "Unable to load the JAWS setup utility dll.");
	ExitOnSpecificValue(getNumberOfJAWSVersionsInstalled(), 0, hr, S_FALSE, "JAWS is not installed.");
	ExitOnSpecificValue(getIndexOfRunningJAWS(), -1, hr, S_FALSE, "JAWS is not active.");
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
	hr = JawsAPI.Invoke0(stopStringDispID, &vFunctionResult);
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
	ExitOnNull(text, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "A message to be spoken was not provided.");
	ExitOnSpecificValue(SysStringLen(text), 0, hr, __HRESULT_FROM_WIN32(ERROR_BAD_ARGUMENTS), "No text has been specified.");
	vText = text;
	hr = CheckAndLoadAPI();
	ExitOnFailure(hr, "The JAWS API could not be loaded.");
	hr = JawsAPI.Invoke2(sayStringDispID, &vText, &vSilence, &vFunctionResult);
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
	hr = JawsAPI.Invoke1(runFunctionDispID, &vBrailleTextFunctionParameter, &vFunctionResult);
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
	ExitOnFalse(isAPILoaded, hr, S_FALSE, "The JAWS API is not loaded.");
LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::LoadAPI()
{
	CComPtr<IDispatch> lpTDispatch =nullptr;
	CComBSTR bJawsAPIClassID = L"FreedomSci.JawsApi";
	HRESULT hr = S_OK;
	//Check to see whether JAWS is active.
	hr = IsProductActive();
	ExitOnFailure(hr, "JAWS is not active.");
	//Now initialize com for the current thread.
hr =	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
ExitOnFailure(hr, "Unable to initialize COM for the current thread.");
hr = lpTDispatch.CoCreateInstance(bJawsAPIClassID);
ExitOnFailure(hr, "Unable to load the JAWS API.");
JawsAPI = lpTDispatch;
hr = JawsAPI.GetIDOfName(L"SayString", &sayStringDispID);
ExitOnFailure(hr, "Unable to find the SayString disp ID.");
hr = JawsAPI.GetIDOfName(L"StopString", &stopStringDispID);
ExitOnFailure(hr, "Unable to find the StopString disp ID.");
hr = JawsAPI.GetIDOfName(L"RunFunction", &stopStringDispID);
ExitOnFailure(hr, "Unable to find the RunFunction disp ID.");
	isAPILoaded = true;
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
	sayStringDispID = -1;
	stopStringDispID = -1;
	runFunctionDispID = -1;
	CoUninitialize();
	isAPILoaded = false;
	LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::IsJAWSSetupUtilityLoaded()
{
	HRESULT hr = S_OK;
	ExitOnFalse(jawsSetupUtilityDllApi.is_loaded(), hr, S_FALSE, "The JAWS setup utility dll is not loaded.");
	ExitOnNull(getNumberOfJAWSVersionsInstalled, hr, S_FALSE, "The GetNumberOfJAWSVersionsInstalled function has not been initialized.");
	ExitOnNull(getIndexOfRunningJAWS, hr, S_FALSE, "The GetIndexOfRunningJAWS function has not been initialized.");
	LExit:
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::LoadJAWSSetupUtility()
{
	HRESULT hr = S_OK;
	LPWSTR pszCurrentProcessPath = nullptr;
	LPWSTR pszCurrentProcessDirectoryPath = nullptr;
	LPWSTR pszJAWSSetupUtilityDllFilePath = nullptr;
	hr = IsJAWSSetupUtilityLoaded();
	ExitOnSuccess(hr, "The JAWS setup utility dll is already loaded.");
	hr = PathForCurrentProcess(&pszCurrentProcessPath, nullptr);
	ExitOnFailure(hr, S_FALSE, "Unable to retrieve the path for the current process.");
	hr = PathGetDirectory(pszCurrentProcessPath, &pszCurrentProcessDirectoryPath);
	ExitOnFailure(hr, "Unable to extract the directory from the path for the current process.");
	hr = PathConcat(pszCurrentProcessDirectoryPath, L"Plugins", &pszJAWSSetupUtilityDllFilePath);
	ExitOnFailure(hr, "Unable to concatinate the main plugins directory path.");
	hr = PathConcat(pszJAWSSetupUtilityDllFilePath, L"JAWS", &pszJAWSSetupUtilityDllFilePath);
	ExitOnFailure(hr, "Unable to concatinate the JAWS plugin directory path.");
	hr = PathConcat(pszJAWSSetupUtilityDllFilePath, L"JAWSSetupUtility.dll", &pszJAWSSetupUtilityDllFilePath);
	ExitOnFailure(hr, "Unable to concatinate the JAWS setup utility dll filename to the JAWS plugin path.");
	ExitOnFalse(FileExistsEx(pszJAWSSetupUtilityDllFilePath, nullptr), hr, E_FILENOTFOUND, "The JAWS setup utility dll could not be found.");
	jawsSetupUtilityDllApi.load(pszJAWSSetupUtilityDllFilePath);
	ExitOnFalse(jawsSetupUtilityDllApi.is_loaded(), hr, S_FALSE, "The JAWS setup utility dll could not be loaded.");
	//Load the functions we need.
	ExitOnFalse(jawsSetupUtilityDllApi.has("GetNumberOfJAWSVersionsInstalled"), hr, S_FALSE, "The JAWS setup utility dll does not export the GetNumberOfJAWSVersionsInstalled function.");
	getNumberOfJAWSVersionsInstalled = jawsSetupUtilityDllApi.get<GetNumberOfJAWSVersionsInstalledFunc>("GetNumberOfJAWSVersionsInstalled");
	ExitOnFalse(jawsSetupUtilityDllApi.has("GetIndexOfRunningJAWS"), hr, S_FALSE, "The JAWS setup utility dll does not export the GetIndexOfRunningJAWS function.");
	getIndexOfRunningJAWS = jawsSetupUtilityDllApi.get<GetIndexOfRunningJAWSFunc>("GetIndexOfRunningJAWS");
	LExit:
	ReleaseNullStr(pszJAWSSetupUtilityDllFilePath);
	ReleaseNullStr(pszCurrentProcessDirectoryPath);
	ReleaseNullStr(pszCurrentProcessPath);
	return hr;
}

HRESULT RaisedBar::RBSpeech::Plugins::CRBSpeechJAWSPlugin::UnloadJAWSSetupUtility()
{
	HRESULT hr = S_OK;
	hr = IsJAWSSetupUtilityLoaded();
	ExitOnFailure(hr, S_FALSE, "The JAWS setup utility dll is not loaded.");
	ExitOnNull(getNumberOfJAWSVersionsInstalled, hr, S_FALSE, "The GetNumberOfJAWSVersionsInstalled function has not been initialized.");
	getNumberOfJAWSVersionsInstalled = nullptr;
	ExitOnNotNull(getNumberOfJAWSVersionsInstalled, hr, S_FALSE, "The GetNumberOfJAWSVersionsInstalled function has not been cleared.");
	ExitOnNull(getIndexOfRunningJAWS, hr, S_FALSE, "The GetIndexOfRunningJAWS function has not been initialized.");
	getIndexOfRunningJAWS = nullptr;
	ExitOnNotNull(getIndexOfRunningJAWS, hr, S_FALSE, "The getIndexOfRunningJAWS function has not been cleared.");
	ExitOnFalse(jawsSetupUtilityDllApi.is_loaded(), hr, S_FALSE, "The JAWS setup utility dll is not loaded.");
	jawsSetupUtilityDllApi.unload();
	ExitOnTrue(jawsSetupUtilityDllApi.is_loaded(), hr, S_FALSE, "The JAWS setup utility dll is still loaded.");
	LExit:
	return hr;
}