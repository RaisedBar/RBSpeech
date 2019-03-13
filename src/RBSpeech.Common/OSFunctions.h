#ifndef OS_FUNCTIONS_H
#define OS_FUNCTIONS_H
#pragma once
#include "stdafx.h"
//WiX includes.
#include <dutil.h>
#include "WiXMacros.h"

namespace RaisedBar
{
	namespace RBSpeech 
{
		namespace OSFunctions
		{
			HRESULT IsAScreenReaderActive();
			HRESULT IsWindowAvailable(__in_z LPCWSTR wzClass, __in_z LPCWSTR wzWindowName);
}
	}
}
#endif // !OS_FUNCTIONS_H