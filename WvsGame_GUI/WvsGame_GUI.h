
// WvsGame_GUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CWvsGame_GUIApp:
// See WvsGame_GUI.cpp for the implementation of this class
//

class CWvsGame_GUIApp : public CWinApp
{
public:
	CWvsGame_GUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWvsGame_GUIApp theApp;
