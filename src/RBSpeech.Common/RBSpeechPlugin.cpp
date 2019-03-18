#include "stdafx.h"
#include "RBSpeechPlugin.h"
HRESULT CRBSpeechPlugin::CheckAndLoadAPI()
{
	HRESULT hr = S_FALSE;
	hr = IsAPILoaded();
	ExitOnSuccess(hr, "The requested API is already loaded.");
	hr = LoadAPI();
	ExitOnFailure(hr, "The requested API could not be loaded.");
LExit:
	return hr;
}
