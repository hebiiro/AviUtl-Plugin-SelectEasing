#include "pch.h"
#include "OutProcess.h"
#include "MainFrame.h"
#include "Common/Tracer2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COutProcessApp theApp;

BEGIN_MESSAGE_MAP(COutProcessApp, CWinApp)
	ON_REGISTERED_THREAD_MESSAGE(WM_SELECT_EASING_NOTIFY, OnSelectEasingNotify)
END_MESSAGE_MAP()

COutProcessApp::COutProcessApp() noexcept
{
	trace_init(0, 0);

	m_mainProcessWindow = 0;
	m_mainProcessId = 0;
}

COutProcessApp::~COutProcessApp()
{
	trace_term();
}

BOOL COutProcessApp::InitInstance()
{
	{
		m_mainProcessWindow = (HWND)_tcstoul(::GetCommandLine(), 0, 0);
		::GetWindowThreadProcessId(m_mainProcessWindow, &m_mainProcessId);

		MY_TRACE_HWND(m_mainProcessWindow);
		HWND parent = ::GetParent(m_mainProcessWindow);
		MY_TRACE_HWND(parent);

#ifdef SELECT_EASING_CHECK_MAIN_PROCESS
		if (!::IsWindow(m_mainProcessWindow))
			return FALSE;
#endif
	}

	CWinApp::InitInstance();

	if (!AfxOleInit())
		return -1;

	EnableTaskbarInteraction(FALSE);

	CFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;
	pFrame->LoadFrame(IDR_MAINFRAME, 0, nullptr, nullptr);

	return TRUE;
}

int COutProcessApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

int COutProcessApp::Run()
{
	GdiplusStartupInput gdiSI;
	GdiplusStartupOutput gdiSO;
	ULONG_PTR gdiToken = 0;
	ULONG_PTR gdiHookToken = 0;

	gdiSI.SuppressBackgroundThread = TRUE;
	GdiplusStartup(&gdiToken, &gdiSI, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);

	int result = CWinApp::Run();

	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiToken);

	return result;
}

void COutProcessApp::OnSelectEasingNotify(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("COutProcessApp::OnSelectEasingNotify(0x%08X, 0x%08X)\n"), wParam, lParam);
#if 1
	m_pMainWnd->PostMessageA(WM_TIMER, TIMER_ID, 0);
#else
	CMainFrame* mainFrame = (CMainFrame*)m_pMainWnd;
	mainFrame->OnTimer(TIMER_ID);
#endif
}
