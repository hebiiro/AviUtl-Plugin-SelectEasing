#include "pch.h"
#include "OutProcess.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL WINAPI WritePrivateProfileInt(
    _In_     LPCTSTR lpAppName,
    _In_     LPCTSTR lpKeyName,
    _In_     INT value,
    _In_opt_ LPCTSTR lpFileName
    )
{
	TCHAR string[MAX_PATH] = {};
	_itot_s(value, string, 10);
	return ::WritePrivateProfileString(lpAppName, lpKeyName, string, lpFileName);
}

CString GetPrivateProfileString(
    _In_opt_ LPCTSTR lpAppName,
    _In_opt_ LPCTSTR lpKeyName,
    _In_opt_ LPCTSTR lpDefault,
    _In_opt_ LPCTSTR lpFileName
    )
{
	CString retValue;
	::GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, retValue.GetBuffer(MAX_PATH), MAX_PATH, lpFileName);
	retValue.ReleaseBuffer();
	return retValue;
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CMainFrame::CMainFrame() noexcept
{
	m_imageVersion = 1;
	m_negative = FALSE;
	m_clamp = FALSE;
	m_horz = _T("left");
	m_vert = _T("center");
	m_alpha = 255;
	m_scale = 100;
	m_x = 0;
	m_y = 0;
	m_selectedColorR = 0xff;
	m_selectedColorG = 0x00;
	m_selectedColorB = 0x00;
	m_selectedColorA = 0x80;
	m_hotColorR = 0x00;
	m_hotColorG = 0xff;
	m_hotColorB = 0x00;
	m_hotColorA = 0x80;

	m_currentPart = -1;
	m_hotPart = -1;
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::loadSettings()
{
	MY_TRACE(_T("CMainFrame::loadSettings()\n"));

	TCHAR path[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), path, MAX_PATH);
	::PathRenameExtension(path, _T(".ini"));

	m_imageVersion = ::GetPrivateProfileInt(_T("viewer"), _T("imageVersion"), m_imageVersion, path);
	m_negative = ::GetPrivateProfileInt(_T("viewer"), _T("negative"), m_negative, path);
	m_clamp = ::GetPrivateProfileInt(_T("viewer"), _T("clamp"), m_clamp, path);
	m_horz = ::GetPrivateProfileString(_T("viewer"), _T("horz"), m_horz, path);
	m_vert = ::GetPrivateProfileString(_T("viewer"), _T("vert"), m_vert, path);
	m_alpha = ::GetPrivateProfileInt(_T("viewer"), _T("alpha"), m_alpha, path);
	m_scale = ::GetPrivateProfileInt(_T("viewer"), _T("scale"), m_scale, path);
	m_x = ::GetPrivateProfileInt(_T("viewer"), _T("x"), m_x, path);
	m_y = ::GetPrivateProfileInt(_T("viewer"), _T("y"), m_y, path);
	m_selectedColorR = ::GetPrivateProfileInt(_T("viewer"), _T("selectedColorR"), m_selectedColorR, path);
	m_selectedColorG = ::GetPrivateProfileInt(_T("viewer"), _T("selectedColorG"), m_selectedColorG, path);
	m_selectedColorB = ::GetPrivateProfileInt(_T("viewer"), _T("selectedColorB"), m_selectedColorB, path);
	m_selectedColorA = ::GetPrivateProfileInt(_T("viewer"), _T("selectedColorA"), m_selectedColorA, path);
	m_hotColorR = ::GetPrivateProfileInt(_T("viewer"), _T("hotColorR"), m_hotColorR, path);
	m_hotColorG = ::GetPrivateProfileInt(_T("viewer"), _T("hotColorG"), m_hotColorG, path);
	m_hotColorB = ::GetPrivateProfileInt(_T("viewer"), _T("hotColorB"), m_hotColorB, path);
	m_hotColorA = ::GetPrivateProfileInt(_T("viewer"), _T("hotColorA"), m_hotColorA, path);

	return TRUE;
}

BOOL CMainFrame::saveSettings()
{
	MY_TRACE(_T("CMainFrame::saveSettings()\n"));

	TCHAR path[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), path, MAX_PATH);
	::PathRenameExtension(path, _T(".ini"));

	CRect rc; GetWindowRect(&rc);

	::WritePrivateProfileInt(_T("viewer"), _T("x"), rc.left, path);
	::WritePrivateProfileInt(_T("viewer"), _T("y"), rc.top, path);

	return TRUE;
}

void CMainFrame::setRect(int number, int _x, int _y)
{
//	MY_TRACE(_T("CMainFrame::setRect(%d, %d, %d)\n"), number, _x, _y);

	switch (m_imageVersion)
	{
	case 2:
		{
			int x = ::MulDiv(_x, m_scale, 100);
			int y = ::MulDiv(_y, m_scale, 100);
			int w = ::MulDiv(_x + 140, m_scale, 100);
			int h = ::MulDiv(_y + 108, m_scale, 100);

			m_parts[number - 1].SetRect(x, y, w, h);

			break;
		}
	default:
		{
			int x = ::MulDiv(_x, m_scale, 100);
			int y = ::MulDiv(_y, m_scale, 100);
			int w = ::MulDiv(_x + 121, m_scale, 100);
			int h = ::MulDiv(_y + 101, m_scale, 100);

			m_parts[number - 1].SetRect(x, y, w, h);

			break;
		}
	}
}

void CMainFrame::setHotPart(int index)
{
	if (m_hotPart == index)
		return;

	m_hotPart = index;
	Invalidate(FALSE);
}

HWND CMainFrame::getTarget()
{
	HWND hwnd = ::GetForegroundWindow();
	DWORD pid = 0;
	DWORD tid = ::GetWindowThreadProcessId(hwnd, &pid);

//	MY_TRACE_HWND(hwnd);
//	MY_TRACE_HEX(pid);
//	MY_TRACE_HEX(theApp.m_mainProcessId);

	if (pid != theApp.m_mainProcessId)
		return 0;

	TCHAR text[MAX_PATH];
	::GetWindowText(hwnd, text, _countof(text));

//	MY_TRACE_STR(text);

	if (::lstrcmp(text, _T("移動フレーム間隔")) != 0)
		return 0;

	return hwnd;
}

int CMainFrame::getEasing()
{
	MY_TRACE(_T("CMainFrame::getEasing()\n"));

	HWND target = ::GetForegroundWindow();
	if (!target)
		return -1;

	MY_TRACE_HWND(target);

	HWND child = ::GetWindow(target, GW_CHILD);
	if (!child)
		return -1;

	MY_TRACE_HWND(child);
	MY_TRACE_NUM(::GetDlgCtrlID(child));

	TCHAR text[MAX_PATH] = {};
	::SendMessage(child, WM_GETTEXT, _countof(text), (LPARAM)text);

	int easing = _ttoi(text);
	if (m_negative)
		easing = -easing - 1;
	else
		easing = easing - 1;
	MY_TRACE_NUM(easing);

	return easing;
}

void CMainFrame::setEasing(int index)
{
	MY_TRACE(_T("CMainFrame::setEasing(%d)\n"), index);

	HWND target = ::GetForegroundWindow();
	if (!target)
		return;

	MY_TRACE_HWND(target);

	int easing = index;
	if (m_negative)
		easing = -easing - 1;
	else
		easing = easing + 1;
	MY_TRACE_NUM(easing);

	HWND child = ::GetWindow(target, GW_CHILD);
	if (!child)
		return;

	MY_TRACE_HWND(child);
	MY_TRACE_NUM(::GetDlgCtrlID(child));

	TCHAR text[MAX_PATH] = {};
	_itot_s(easing, text, 10);
	::SendMessage(child, WM_SETTEXT, 0, (LPARAM)text);
	::PostMessage(target, WM_COMMAND, IDOK, 0);
}

void CMainFrame::show(HWND target)
{
	if (IsWindowVisible())
		return;

	m_currentPart = getEasing();
	m_hotPart = -1;

	CRect rc; GetWindowRect(&rc);
	CRect rcTarget; ::GetWindowRect(target, &rcTarget);

	int x, y;

	if (::lstrcmpi(m_horz, _T("left")) == 0)
	{
		x = rcTarget.left - rc.Width();
	}
	else if (::lstrcmpi(m_horz, _T("right")) == 0)
	{
		x = rcTarget.right;
	}
	else
	{
		x = rcTarget.CenterPoint().x - rc.Width() / 2;
	}

	if (::lstrcmpi(m_vert, _T("top")) == 0)
	{
		y = rcTarget.top - rc.Height();
	}
	else if (::lstrcmpi(m_vert, _T("bottom")) == 0)
	{
		y = rcTarget.bottom;
	}
	else
	{
		y = rcTarget.CenterPoint().y - rc.Height() / 2;
	}

	if (m_clamp)
	{
		HMONITOR monitor = ::MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) }; ::GetMonitorInfo(monitor, &mi);

		if (x < mi.rcWork.left)
			x = mi.rcWork.left;
		else if (x + rc.Width() > mi.rcWork.right)
			x = mi.rcWork.right - rc.Width();

		if (y < mi.rcWork.top)
			y = mi.rcWork.top;
		else if (y + rc.Height() > mi.rcWork.bottom)
			y = mi.rcWork.bottom - rc.Height();
	}

	SetWindowPos(&wndTopMost, x, y, 0, 0,
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	Invalidate(FALSE);
}

void CMainFrame::hide()
{
	if (!IsWindowVisible())
		return;

	ShowWindow(SW_HIDE);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style = WS_POPUP | WS_CAPTION | WS_SYSMENU | FWS_ADDTOTITLE;
	cs.dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	MY_TRACE(_T("CMainFrame::OnCreate()\n"));

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!loadSettings())
		return -1;

	SetLayeredWindowAttributes(0, m_alpha, LWA_ALPHA);

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(AfxGetInstanceHandle(), path, _countof(path));
	::PathRemoveFileSpec(path);

	switch (m_imageVersion)
	{
	case 2:
		{
			::PathAppend(path, _T("easing2.png"));
			MY_TRACE_STR(path);

			m_image.Load(path);

			if (!m_image.IsNull())
			{
				int w = m_image.GetWidth() * m_scale / 100;
				int h = m_image.GetHeight() * m_scale / 100;

				MoveWindow(m_x, m_y, w, h);
			}

			m_parts.resize(41);

			int x = 20;
			int y = 98;

			setRect(1, x, y);

			for (int i = 0; i < 5; i++)
			{
				y += 155;

				int x1 = 20;
				int x2 = 695;

				for (int j = 0; j < 4; j++)
				{
					setRect((i * 8) + j + 2 + 0, x1, y);
					setRect((i * 8) + j + 2 + 4, x2, y);

					x1 += 163;
					x2 += 163;
				}
			}

			break;
		}
	default:
		{
			::PathAppend(path, _T("easing.png"));
			MY_TRACE_STR(path);

			m_image.Load(path);

			if (!m_image.IsNull())
			{
				int w = m_image.GetWidth() * m_scale / 100;
				int h = m_image.GetHeight() * m_scale / 100;

				MoveWindow(m_x, m_y, w, h);
			}

			m_parts.resize(41);

			int x = 103;
			int y = 40;

			setRect(1, x, y);

			for (int i = 0; i < 5; i++)
			{
				y += 178;

				int x1 = 103;
				int x2 = 649;

				for (int j = 0; j < 4; j++)
				{
					setRect((i * 8) + j + 2 + 0, x1, y);
					setRect((i * 8) + j + 2 + 4, x2, y);

					x1 += 125;
					x2 += 125;
				}
			}

			break;
		}
	}

	SetTimer(TIMER_ID, 1000, 0);

	return 0;
}

void CMainFrame::OnDestroy()
{
	MY_TRACE(_T("CMainFrame::OnDestroy()\n"));

	KillTimer(TIMER_ID);

	m_image.Destroy();

	saveSettings();

	CFrameWnd::OnDestroy();
}

void CMainFrame::OnPaint()
{
	CPaintDC dc(this);
	CRect rc; GetClientRect(&rc);

	if (!m_image.IsNull())
	{
		int w = m_image.GetWidth();
		int h = m_image.GetHeight();
		m_image.AlphaBlend(dc, rc.left, rc.top, rc.Width(), rc.Height(), 0, 0, w, h);
	}

	Graphics g(dc);
	g.SetSmoothingMode(SmoothingModeAntiAlias);
//	g.SetCompositingMode(CompositingModeSourceOver);
	g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g.TranslateTransform(-0.5f, -0.5f);

	if (m_currentPart >= 0 && m_currentPart < (int)m_parts.size())
	{
		SolidBrush brush(Color(m_selectedColorA, m_selectedColorR, m_selectedColorG, m_selectedColorB));
		REAL x = (REAL)m_parts[m_currentPart].left;
		REAL y = (REAL)m_parts[m_currentPart].top;
		REAL w = (REAL)m_parts[m_currentPart].Width();
		REAL h = (REAL)m_parts[m_currentPart].Height();

		g.FillRectangle(&brush, x, y, w, h);
	}

	if (m_hotPart >= 0 && m_hotPart < (int)m_parts.size())
	{
		SolidBrush brush(Color(m_hotColorA, m_hotColorR, m_hotColorG, m_hotColorB));
		REAL x = (REAL)m_parts[m_hotPart].left;
		REAL y = (REAL)m_parts[m_hotPart].top;
		REAL w = (REAL)m_parts[m_hotPart].Width();
		REAL h = (REAL)m_parts[m_hotPart].Height();

		g.FillRectangle(&brush, x, y, w, h);
	}
}

void CMainFrame::OnTimer(UINT_PTR timerId)
{
	if (timerId == TIMER_ID)
	{
#ifdef SELECT_EASING_CHECK_MAIN_PROCESS
		if (!::IsWindow(theApp.m_mainProcessWindow))
		{
			KillTimer(TIMER_ID);
			PostMessage(WM_CLOSE);
		}
#endif
		HWND target = getTarget();

		if (target)
		{
			show(target);
		}
		else
		{
			hide();
		}
	}

	CFrameWnd::OnTimer(timerId);
}

BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	return FALSE;
//	return CFrameWnd::OnNcActivate(bActive);
}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
//	CFrameWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	ScreenToClient(&point);

	for (size_t i = 0; i < m_parts.size(); i++)
	{
		if (m_parts[i].PtInRect(point))
		{
			setHotPart(i);
			return HTCLIENT;
		}
	}

	setHotPart(-1);
	return HTCAPTION;
//	return CFrameWnd::OnNcHitTest(point);
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (size_t i = 0; i < m_parts.size(); i++)
	{
		if (m_parts[i].PtInRect(point))
		{
			setEasing(i);

			return;
		}
	}

	CFrameWnd::OnLButtonDown(nFlags, point);
}
