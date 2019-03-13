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

}
	}
}
#endif // !OS_FUNCTIONS_H