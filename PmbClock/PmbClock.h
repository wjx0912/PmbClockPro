
// PmbClock.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPmbClockApp:
// See PmbClock.cpp for the implementation of this class
//

class CPmbClockApp : public CWinApp
{
public:
	CPmbClockApp();


private:
	HANDLE m_hMutex;

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPmbClockApp theApp;
