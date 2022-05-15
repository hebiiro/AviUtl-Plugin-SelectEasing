#include "pch.h"
#include "OutProcess.h"
#include "EaseWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------

CEaseWindow::CEaseWindow()
{
	m_points[Points::first].x = Position::end;
	m_points[Points::first].y = Position::start;
	m_points[Points::second].x = Position::start;
	m_points[Points::second].y = Position::end;
	m_hot = Points::none;

	m_enable = TRUE;
	m_origin = _T("easing");
	m_clamp = TRUE;
	m_horz = _T("left");
	m_vert = _T("center");
	m_alpha = 255;
	m_margin = 32;
	m_hitDistance = 0;
	m_windowSize.cx = 400;
	m_windowSize.cy = 400;
	m_backgroundColor = Color(255, 255, 255, 255);
	m_borderColor = Color(255, 224, 224, 224);
	m_borderWidth = 4.0f;
	m_curveColor = Color(255, 0, 0, 0);
	m_curveWidth = 4.0f;
	m_handleColor = Color(255, 128, 192, 255);
	m_handleWidth = 4.0f;
	m_pointColor = Color(255, 0, 128, 255);
	m_hotPointColor = Color(255, 224, 0, 0);
	m_pointRadius = 16;
	m_hideCursor = FALSE;
}

CEaseWindow::~CEaseWindow()
{
}

void CEaseWindow::getWorkarea(LPRECT rc)
{
	GetClientRect(rc);
	::InflateRect(rc, -m_margin, -m_margin);
}

POINT CEaseWindow::LPToClient(POINT point)
{
	CRect rc; getWorkarea(&rc);
	return LPToClient(point, &rc);
}

POINT CEaseWindow::LPToClient(POINT point, LPCRECT rc)
{
	int cw = rc->right - rc->left;
	int ch = rc->bottom - rc->top;
	int lw = Position::end - Position::start;
	int lh = Position::end - Position::start;

	double fx = (double)(point.x - Position::start) / lw;
	double fy = (double)(point.y - Position::start) / lh;

	fy = 1.0 - fy;

	return CPoint(
		rc->left + (int)(cw * fx),
		rc->top  + (int)(ch * fy));
}

POINT CEaseWindow::ClientToLP(POINT point)
{
	CRect rc; getWorkarea(&rc);
	return ClientToLP(point, &rc);
}

POINT CEaseWindow::ClientToLP(POINT point, LPCRECT rc)
{
	int cw = rc->right - rc->left;
	int ch = rc->bottom - rc->top;
	int lw = Position::end - Position::start;
	int lh = Position::end - Position::start;

	double fx = (double)(point.x - rc->left) / cw;
	double fy = (double)(point.y - rc->top) / ch;

	fy = 1.0 - fy;

	CPoint retValue(
		Position::start + (int)round(lw * fx),
		Position::start + (int)round(lh * fy));

	retValue.x = max(retValue.x, Position::start);
	retValue.x = min(retValue.x, Position::end);
	retValue.y = max(retValue.y, Position::start);
	retValue.y = min(retValue.y, Position::end);

	return retValue;
}

int CEaseWindow::hitTest(POINT point)
{
	CRect rc; getWorkarea(&rc);

	for (int i = 0; i < Points::maxSize; i++)
	{
		CPoint point2 = LPToClient(m_points[i], &rc);

		if (abs(point.x - point2.x) <= m_hitDistance + m_pointRadius &&
			abs(point.y - point2.y) <= m_hitDistance + m_pointRadius)
		{
			return i;
		}
	}

	return Points::none;
}

void CEaseWindow::outputEaseText()
{
	CString text;
	text.Format(_T("(%d, %d) (%d, %d) - %02d%02d%02d%02d"),
		m_points[Points::first].x,
		m_points[Points::first].y,
		m_points[Points::second].x,
		m_points[Points::second].y,
		m_points[Points::first].x,
		m_points[Points::first].y,
		m_points[Points::second].x,
		m_points[Points::second].y);

	SetWindowText(text);
}

void CEaseWindow::receiveNumber()
{
	MY_TRACE(_T("CEaseWindow::receiveNumber()\n"));

	HWND target = ::GetForegroundWindow();
	if (!target) return;
	MY_TRACE_HWND(target);

	HWND child = ::GetWindow(target, GW_CHILD);
	if (!child) return;
	MY_TRACE_HWND(child);
	MY_TRACE_INT(::GetDlgCtrlID(child));

	TCHAR text[MAX_PATH] = {};
	::SendMessage(child, WM_GETTEXT, _countof(text), (LPARAM)text);
	MY_TRACE_TSTR(text);

	if (::lstrlen(text) != 8) return;

	TCHAR buffer[3] = {};
	memcpy(buffer, text + 0, sizeof(TCHAR) * 2); m_points[Points::first].x = _ttoi(buffer);
	memcpy(buffer, text + 2, sizeof(TCHAR) * 2); m_points[Points::first].y = _ttoi(buffer);
	memcpy(buffer, text + 4, sizeof(TCHAR) * 2); m_points[Points::second].x = _ttoi(buffer);
	memcpy(buffer, text + 6, sizeof(TCHAR) * 2); m_points[Points::second].y = _ttoi(buffer);
}

void CEaseWindow::sendNumber()
{
	MY_TRACE(_T("CEaseWindow::sendNumber()\n"));

	HWND target = ::GetForegroundWindow();
	if (!target)
		return;

	MY_TRACE_HWND(target);

	HWND child = ::GetWindow(target, GW_CHILD);
	if (!child)
		return;

	MY_TRACE_HWND(child);
	MY_TRACE_INT(::GetDlgCtrlID(child));

	TCHAR text[MAX_PATH] = {};
	::StringCbPrintf(text, sizeof(text), _T("%02d%02d%02d%02d"),
		m_points[Points::first].x, m_points[Points::first].y,
		m_points[Points::second].x, m_points[Points::second].y);
	::SendMessage(child, WM_SETTEXT, 0, (LPARAM)text);
	::PostMessage(target, WM_COMMAND, IDOK, 0);
}

void CEaseWindow::show(HWND numberWindow, HWND easingWindow)
{
	MY_TRACE(_T("CEaseWindow::show(0x%08X, 0x%08X)\n"), numberWindow, easingWindow);

	if (!m_enable) return;
	if (IsWindowVisible()) return;

	// ターゲットを取得する。
	HWND target = numberWindow;
	if (::lstrcmpi(m_origin, _T("easing")) == 0)
		target = easingWindow;
	MY_TRACE_HWND(target);

	// 現在値を受け取る。
	receiveNumber();
	outputEaseText();

	// ホット状態を初期化する。
	m_hot = Points::none;

	// 幅と高さを取得する。
	int w, h;
	{
		// クライアントサイズを設定する。
		CRect rc(0, 0, m_windowSize.cx, m_windowSize.cy);
		getWindowRectFromClientRect(GetSafeHwnd(), &rc);
		w = rc.Width();
		h = rc.Height();
	}

	// x と y を取得する。
	int x, y;
	{
		CRect rcTarget; ::GetWindowRect(target, &rcTarget);

		if (::lstrcmpi(m_horz, _T("left")) == 0)
		{
			x = rcTarget.left - w;
		}
		else if (::lstrcmpi(m_horz, _T("right")) == 0)
		{
			x = rcTarget.right;
		}
		else
		{
			x = rcTarget.CenterPoint().x - w / 2;
		}

		if (::lstrcmpi(m_vert, _T("top")) == 0)
		{
			y = rcTarget.top - h;
		}
		else if (::lstrcmpi(m_vert, _T("bottom")) == 0)
		{
			y = rcTarget.bottom;
		}
		else
		{
			y = rcTarget.CenterPoint().y - h / 2;
		}

		if (m_clamp)
		{
			HMONITOR monitor = ::MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) }; ::GetMonitorInfo(monitor, &mi);

			if (x < mi.rcWork.left)
				x = mi.rcWork.left;
			else if (x + w > mi.rcWork.right)
				x = mi.rcWork.right - w;

			if (y < mi.rcWork.top)
				y = mi.rcWork.top;
			else if (y + h > mi.rcWork.bottom)
				y = mi.rcWork.bottom - h;
		}
	}

	SetLayeredWindowAttributes(0, m_alpha, LWA_ALPHA);
	SetWindowPos(&wndTopMost, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
	Invalidate(FALSE);
}

void CEaseWindow::hide()
{
	MY_TRACE(_T("CEaseWindow::hide()\n"));

	if (!IsWindowVisible()) return;

	ShowWindow(SW_HIDE);
}

//--------------------------------------------------------------------

BOOL CEaseWindow::Create(CWnd* parent)
{
	MY_TRACE(_T("CEaseWindow::Create(0x%08X)\n"), parent);

	return CWnd::CreateEx(0, 0, 0, WS_POPUP, CRect(0, 0, 0, 0), parent, 0);
}

BOOL CEaseWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	MY_TRACE(_T("CEaseWindow::PreCreateWindow()\n"));

	cs.style = WS_POPUP | WS_CAPTION | WS_THICKFRAME;
	cs.dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED;
	cs.lpszClass = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(0, IDC_ARROW));

	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

BEGIN_MESSAGE_MAP(CEaseWindow, CWnd)
	ON_WM_PAINT()
	ON_WM_NCACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CEaseWindow::OnPaint() 
{
	CPaintDC paintDC(this);

	CUxDC dc(paintDC);
	if (!dc.isValid()) return;

	Graphics g(dc);

	g.SetSmoothingMode(SmoothingModeAntiAlias);
//	g.SetCompositingMode(CompositingModeSourceOver);
//	g.SetTextRenderingHint(TextRenderingHintAntiAlias);
	g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
//	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
//	g.TranslateTransform(-0.5f, -0.5f);

	g.Clear(m_backgroundColor);

	CRect rc; getWorkarea(&rc);
	CRect rcPaint = rc;
	rcPaint.right--; rcPaint.bottom--;

	{
		// 外枠を描画する。

		Pen pen(m_borderColor, m_borderWidth);
		Rect rect;
		rect.X = rcPaint.left;
		rect.Y = rcPaint.top;
		rect.Width = rcPaint.Width();
		rect.Height = rcPaint.Height();
		g.DrawRectangle(&pen, rect);
	}

	Point start(rcPaint.left, rcPaint.bottom);
	Point end(rcPaint.right, rcPaint.top);
	MyPoint first(LPToClient(m_points[Points::first], &rcPaint));
	MyPoint second(LPToClient(m_points[Points::second], &rcPaint));

	{
		// ハンドルを描画する。

		Pen pen(m_handleColor, m_handleWidth);
		g.DrawLine(&pen, start, first);
		g.DrawLine(&pen, second, end);
	}

	{
		// 制御点を描画する。

		SolidBrush brush(m_pointColor);
		SolidBrush hotBrush(m_hotPointColor);

		for (int i = 0; i < Points::maxSize; i++)
		{
			CPoint point = LPToClient(m_points[i]);

			Rect rect;
			rect.X = point.x - m_pointRadius;
			rect.Y = point.y - m_pointRadius;
			rect.Width = m_pointRadius * 2;
			rect.Height = m_pointRadius * 2;

			if (i == m_hot)
				g.FillEllipse(&hotBrush, rect);
			else
				g.FillEllipse(&brush, rect);
		}
	}

	{
		// ベジェ曲線を描画する。

		Pen pen(m_curveColor, m_curveWidth);
		g.DrawBezier(&pen, start, first, second, end);
	}
}

BOOL CEaseWindow::OnNcActivate(BOOL bActive)
{
	return FALSE;
//	return CWnd::OnNcActivate(bActive);
}

void CEaseWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	MY_TRACE(_T("CEaseWindow::OnLButtonDown(0x%08X, %d, %d)\n"), nFlags, point.x, point.y);

	m_hot = hitTest(point);
	if (m_hot != Points::none)
	{
		// ドラッグを開始する。

		SetCapture();
		if (m_hideCursor) ::ShowCursor(FALSE);
		Invalidate(FALSE);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CEaseWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	MY_TRACE(_T("CEaseWindow::OnLButtonUp(0x%08X, %d, %d)\n"), nFlags, point.x, point.y);

	if (GetCapture() == this)
	{
		// ドラッグを終了する。

		m_hot = Points::none;
		ReleaseCapture();
		Invalidate(FALSE);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CEaseWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	MY_TRACE(_T("CEaseWindow::OnLButtonDblClk(0x%08X, %d, %d)\n"), nFlags, point.x, point.y);

	sendNumber();

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CEaseWindow::OnRButtonDown(UINT nFlags, CPoint point)
{
	MY_TRACE(_T("CEaseWindow::OnRButtonDown(0x%08X, %d, %d)\n"), nFlags, point.x, point.y);

	CWnd::OnRButtonDown(nFlags, point);
}

void CEaseWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
	MY_TRACE(_T("CEaseWindow::OnRButtonUp(0x%08X, %d, %d)\n"), nFlags, point.x, point.y);

	CWnd::OnRButtonUp(nFlags, point);
}

void CEaseWindow::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	MY_TRACE(_T("CEaseWindow::OnRButtonDblClk(0x%08X, %d, %d)\n"), nFlags, point.x, point.y);

	CWnd::OnRButtonDblClk(nFlags, point);
}

void CEaseWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		// ドラッグ中。

		if (m_hot >= 0 && m_hot < Points::maxSize)
		{
			m_points[m_hot] = ClientToLP(point);
			Invalidate(FALSE);

			outputEaseText();
		}
	}
	else
	{
		// ドラッグ中ではない。

		int hot = hitTest(point);
		if (m_hot != hot)
		{
			m_hot = hot;
			Invalidate(FALSE);

			TRACKMOUSEEVENT tme = { sizeof(tme) };
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = GetSafeHwnd();
			::TrackMouseEvent(&tme);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CEaseWindow::OnMouseLeave()
{
	MY_TRACE(_T("CEaseWindow::OnMouseLeave()\n"));

	// ホット状態を解除する。
	m_hot = Points::none;
	Invalidate(FALSE);

	CWnd::OnMouseLeave();
}

void CEaseWindow::OnCaptureChanged(CWnd* pWnd)
{
	MY_TRACE(_T("CEaseWindow::OnCaptureChanged()\n"));

	if (m_hideCursor) ::ShowCursor(TRUE);

	CWnd::OnCaptureChanged(pWnd);
}

void CEaseWindow::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	Invalidate(FALSE);
}

//--------------------------------------------------------------------
