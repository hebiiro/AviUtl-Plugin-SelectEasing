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
	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);
	BOOL proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip);
	BOOL WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp);

	BOOL createSubProcess();

	void initHook();
	void termHook();

	void update(int value);
};

extern CInProcessApp theApp;
