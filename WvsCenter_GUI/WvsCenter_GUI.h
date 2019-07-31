
// WvsCenter_GUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CWvsCenter_GUIApp:
// See WvsCenter_GUI.cpp for the implementation of this class
//

class CWvsCenter_GUIApp : public CWinApp
{
public:
	CWvsCenter_GUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWvsCenter_GUIApp theApp;