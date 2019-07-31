
// WvsLogin_GUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CWvsLogin_GUIApp:
// See WvsLogin_GUI.cpp for the implementation of this class
//

class CWvsLogin_GUIApp : public CWinApp
{
public:
	CWvsLogin_GUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWvsLogin_GUIApp theApp;