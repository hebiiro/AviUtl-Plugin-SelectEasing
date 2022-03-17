#pragma once

#include "resource.h"

const UINT WM_SELECT_EASING_NOTIFY = ::RegisterWindowMessage(_T("WM_SELECT_EASING_NOTIFY"));

const UINT_PTR TIMER_ID = 1;

class COutProcessApp : public CWinApp
{
public:

	HWND m_mainProcessWindow;
	DWORD m_mainProcessId;

public:

	COutProcessApp() noexcept;
	virtual ~COutProcessApp();

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	afx_msg void OnSelectEasingNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};

extern COutProcessApp theApp;
