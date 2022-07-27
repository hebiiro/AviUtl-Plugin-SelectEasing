#include "pch.h"
#include "InProcess.h"
#include "InProcess_Hook.h"

CInProcessApp theApp;

void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

CInProcessApp::CInProcessApp()
{
	m_instance = 0;
	m_hwnd = 0;
	::ZeroMemory(&m_pi, sizeof(m_pi));
}

CInProcessApp::~CInProcessApp()
{
}

BOOL CInProcessApp::dllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			MY_TRACE(_T("DLL_PROCESS_ATTACH\n"));

			m_instance = instance;
			MY_TRACE_HEX(m_instance);

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			MY_TRACE(_T("DLL_PROCESS_DETACH\n"));

			break;
		}
	}

	return TRUE;
}

BOOL CInProcessApp::init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("CInProcessApp::init()\n"));

	m_hwnd = fp->hwnd;

	if (!createSubProcess())
		return FALSE;

	m_auin.initExEditAddress();

	initHook();

	return TRUE;
}

BOOL CInProcessApp::exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("CInProcessApp::exit()\n"));

	::CloseHandle(m_pi.hThread);
	::CloseHandle(m_pi.hProcess);

	termHook();

	return TRUE;
}

BOOL CInProcessApp::proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
{
	MY_TRACE(_T("CInProcessApp::proc()\n"));

	return TRUE;
}

BOOL CInProcessApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
{
//	MY_TRACE(_T("CInProcessApp::WndProc(0x%08X, 0x%08X, 0x%08X)\n"), message, wParam, lParam);

	switch (message)
	{
	case WM_SELECT_EASING_UPDATE:
		{
			MY_TRACE(_T("CInProcessApp::WndProc(WM_SELECT_EASING_UPDATE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			update(wParam);

			break;
		}
	case WM_WINDOWPOSCHANGING:
		{
			MY_TRACE(_T("CInProcessApp::WndProc(WM_WINDOWPOSCHANGING, 0x%08X, 0x%08X)\n"), wParam, lParam);

			::PostThreadMessage(m_pi.dwThreadId, WM_SELECT_EASING_NOTIFY, 0, 0);

			break;
		}
	}

	return FALSE;
}

BOOL CInProcessApp::createSubProcess()
{
	MY_TRACE(_T("CInProcessApp::createSubProcess()\n"));

	TCHAR fileSpec[MAX_PATH] = {};
	::GetModuleFileName(m_instance, fileSpec, MAX_PATH);
	::PathStripPath(fileSpec);
	MY_TRACE_TSTR(fileSpec);

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(m_instance, path, MAX_PATH);
	::PathRemoveExtension(path);
	::PathAppend(path, fileSpec);
	::PathRenameExtension(path, _T(".exe"));
	MY_TRACE_STR(path);

	TCHAR args[MAX_PATH] = {};
	::StringCbPrintf(args, sizeof(args), _T("0x%08p"), m_hwnd);
	MY_TRACE_STR(args);

	STARTUPINFO si = { sizeof(si) };
	if (!::CreateProcess(
		path,           // No module name (use command line)
		args,           // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&m_pi))         // Pointer to PROCESS_INFORMATION structur
	{
		MY_TRACE(_T("::CreateProcess() が失敗しました\n"));

		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------

// フックをセットする。
void CInProcessApp::initHook()
{
	MY_TRACE(_T("CInProcessApp::initHook()\n"));

	DWORD exedit = theApp.m_auin.GetExEdit();
	m_trackTable = (int*)(exedit + 0x14E900);
	true_GetParamSmallExternal = hookCall(exedit + 0x2DBB1, hook_GetParamSmallExternal);
}

// フックを解除する。
void CInProcessApp::termHook()
{
	MY_TRACE(_T("CInProcessApp::termHook()\n"));
}

//--------------------------------------------------------------------

void CInProcessApp::update(int value)
{
	MY_TRACE(_T("update(%d)\n"), value);

	if (!m_trackOffsets)
		return;

	m_trackTable[m_trackIndex] = value;

	{
		int offset = m_trackOffsets[0];
		if (offset) m_trackTable[m_trackIndex + offset] = value;
	}

	{
		int offset = m_trackOffsets[1];
		if (offset) m_trackTable[m_trackIndex + offset] = value;
	}

	::SendMessage(m_auin.GetExEditWindow(), 0x111, 0x3EB, m_trackIndex | 0x00080000);
}

//--------------------------------------------------------------------
