#pragma once

class CMainFrame : public CFrameWnd
{
public:

	typedef std::vector<CRect> CParts;

public:

	BOOL m_negative;
	BOOL m_left;
	int m_alpha;
	int m_scale;
	int m_x, m_y;
	CImage m_image;
	CParts m_parts;
	int m_currentPart;
	int m_hotPart;

public:

	CMainFrame() noexcept;
	virtual ~CMainFrame();

	BOOL loadSettings();
	BOOL saveSettings();
	void setRect(int number, int x, int y);
	void setHotPart(int index);
	HWND getTarget();
	int getEasing();
	void setEasing(int index);
	void show(HWND target);
	void hide();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};
