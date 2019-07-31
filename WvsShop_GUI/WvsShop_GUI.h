
// WvsShop_GUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CWvsShop_GUIApp:
// See WvsShop_GUI.cpp for the implementation of this class
//

class CWvsShop_GUIApp : public CWinApp
{
public:
	CWvsShop_GUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWvsShop_GUIApp theApp;