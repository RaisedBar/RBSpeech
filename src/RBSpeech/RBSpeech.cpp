// RBSpeech.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "RBSpeech_i.h"
#include "xdlldata.h"


using namespace ATL;


class CRBSpeechModule : public ATL::CAtlExeModuleT< CRBSpeechModule >
{
public :
	DECLARE_LIBID(LIBID_RBSpeechLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_RBSPEECH, "{79932615-cff4-4a76-b510-dfea8e864b2d}")
};

CRBSpeechModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

