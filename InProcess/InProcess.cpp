#include "pch.h"
#include "InProcess.h"

const UINT WM_SELECT_EASING_NOTIFY = ::RegisterWindowMessage(_T("WM_SELECT_EASING_NOTIFY"));

CInProcessApp theApp;

void ___outputLog(LPCTSTR text, LPCTSTR output)
{
}

CInProcessApp::CInProcessApp()
{
	::ZeroMemory(m_name, sizeof(m_name));
	::ZeroMemory(m_information, sizeof(m_information));
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

			::GetModuleFileName(m_instance, m_name, MAX_PATH);
			::PathStripPath(m_name);
			::PathRemoveExtension(m_name);
			MY_TRACE_STR(m_name);

			::StringCbCopy(m_information, sizeof(m_information), _T("イージング選択 Version 3.0.0 by 蛇色"));
			MY_TRACE_STR(m_information);

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

BOOL CInProcessApp::init(FILTER *fp)
{
	MY_TRACE(_T("CInProcessApp::init()\n"));

	m_hwnd = fp->hwnd;

	if (!createSubProcess())
		return FALSE;

	return TRUE;
}

BOOL CInProcessApp::exit(FILTER *fp)
{
	MY_TRACE(_T("CInProcessApp::exit()\n"));

	::CloseHandle(m_pi.hThread);
	::CloseHandle(m_pi.hProcess);

	return TRUE;
}

BOOL CInProcessApp::proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	MY_TRACE(_T("CInProcessApp::proc()\n"));

	return TRUE;
}

BOOL CInProcessApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, void *editp, FILTER *fp)
{
	MY_TRACE(_T("CInProcessApp::WndProc(0x%08X, 0x%08X, 0x%08X)\n"), message, wParam, lParam);

	switch (message)
	{
	case WM_WINDOWPOSCHANGING:
		{
			MY_TRACE(_T("CInProcessApp::WndProc(WM_WINDOWPOSCHANGING, 0x%08X, 0x%08X)\n"), wParam, lParam);

			::PostThreadMessage(m_pi.dwThreadId, WM_SELECT_EASING_NOTIFY, 0, 0);

			break;
		}
	case WM_FILTER_CHANGE_EDIT:
		{
			MY_TRACE(_T("CInProcessApp::WndProc(WM_FILTER_CHANGE_EDIT)\n"));

			if (fp->exfunc->is_editing(editp) != TRUE) break;

			break;
		}
	case WM_FILTER_CHANGE_WINDOW:
		{
			MY_TRACE(_T("CInProcessApp::WndProc(WM_FILTER_CHANGE_WINDOW)\n"));

			break;
		}
	}

	return FALSE;
}

BOOL CInProcessApp::createSubProcess()
{
	MY_TRACE(_T("CInProcessApp::createSubProcess()\n"));

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(m_instance, path, MAX_PATH);
	::PathRemoveExtension(path);
	::PathAppend(path, m_name);
	::PathAddExtension(path, _T(".exe"));

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
		MY_TRACE(_T("::CreateProcess() failed.\n"));

		return FALSE;
	}

	return TRUE;
}
