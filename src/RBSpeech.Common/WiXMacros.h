#ifndef WIX_MACROS_H
#define WIX_MACROS_H
#pragma once
#include "stdafx.h"
#include <dutil.h>
#pragma once
// Copyright (c) .NET Foundation and contributors. All rights reserved. Licensed under the Microsoft Reciprocal License. See LICENSE.TXT file in the project root for full license information.
#ifdef __cplusplus
extern "C" {
#endif

//macros to extend the wix system, to exit on false and exit if values are equal or not equal to a given value.
#define ExitOnFalse(v, x, e, s)   if (v ==false) { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitOnFalseWithLastError(v, x, s) {if (v ==false) { DWORD Dutil_er = ::GetLastError(); x = HRESULT_FROM_WIN32(Dutil_er); if (FAILED(x)) { Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s); goto LExit; } } }
#define ExitOnTrue(v, x, e, s)   if (v ==true) { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitOnSpecificValue(p, v, x, e, s)   if (v == p) { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitIfValueGreaterThanSuppliedValue(p, v, x, e, s)   if (v > p) { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitIfValueLessThanOrEqualToSuppliedValue(p, v, x, e, s)   if (v <= p) { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitIfValueLessThanSuppliedValue(p, v, x, e, s)   if (v < p) { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitIfValueNotEqualToSuppliedValue(p, v, x, e, s)   if (v != p) { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitFunctionWithMessage(x, s)   { ExitTrace(x, s);  goto LExit; }
#define ExitFunctionWithMessageAndStatusCode(x, e, s)  { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitOnNotNull(p, x, e, s)   if (NULL != p) { x = e; Dutil_RootFailure(__FILE__, __LINE__, x); ExitTrace(x, s);  goto LExit; }
#define ExitOnSuccess(x, s, ...)   if (SUCCEEDED(x)) { ExitTrace(x, s, __VA_ARGS__);  goto LExit; }
#ifdef __cplusplus
}
#endif
	#endif // !WIX_MACROS_H