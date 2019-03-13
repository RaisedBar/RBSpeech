#include "stdafx.h"
#include "OSFunctions.h"

HRESULT RaisedBar::RBSpeech::OSFunctions::IsScreenReaderActive()
{
	HRESULT hr = S_OK;
	bool bResult = false;
	//Determine whether a screen reader is running.
	bool bIsScreenReaderActive = false;
	bResult = SystemParametersInfo(SPI_GETSCREENREADER, 0, &bIsScreenReaderActive, 0);
	ExitOnFalseWithLastError(bResult, hr, "An error occured whilst evaluating whether a screen reader is active.");
	ExitOnFalse(bIsScreenReaderActive, hr, S_FALSE, "A screen reader is not running.");
LExit:
	return hr;
}