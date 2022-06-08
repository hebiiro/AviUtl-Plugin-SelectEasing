#pragma once

class CInProcessApp
{
public:

	HINSTANCE m_instance;
	HWND m_hwnd;
	PROCESS_INFORMATION m_pi;

	AviUtlInternal m_auin;
	int* m_trackTable = 0;
	int* m_trackOffsets = 0;
	int m_trackIndex = 0;

public:

	CInProcessApp();
	~CInProcessApp();

	BOOL dllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
	BOOL init(FILTER *fp);
	BOOL exit(FILTER *fp);
	BOOL proc(FILTER *fp, FILTER_PROC_INFO *fpip);
	BOOL WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, void *editp, FILTER *fp);

	BOOL createSubProcess();

	void initHook();
	void termHook();

	void update(int value);
};

extern CInProcessApp theApp;
