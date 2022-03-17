#pragma once

class CInProcessApp
{
public:

	TCHAR m_name[MAX_PATH];
	TCHAR m_information[MAX_PATH];

	HINSTANCE m_instance;
	HWND m_hwnd;
	PROCESS_INFORMATION m_pi;

public:

	CInProcessApp();
	~CInProcessApp();

	BOOL dllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
	BOOL init(FILTER *fp);
	BOOL exit(FILTER *fp);
	BOOL proc(FILTER *fp, FILTER_PROC_INFO *fpip);
	BOOL WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, void *editp, FILTER *fp);

	BOOL createSubProcess();
};

extern CInProcessApp theApp;
