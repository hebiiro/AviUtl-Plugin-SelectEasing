#pragma once

#include "resource.h"

//--------------------------------------------------------------------

const UINT WM_SELECT_EASING_NOTIFY = ::RegisterWindowMessage(_T("WM_SELECT_EASING_NOTIFY"));

const UINT_PTR TIMER_ID = 1;

//--------------------------------------------------------------------

inline HRESULT WINAPI getPrivateProfileColor(
	const MSXML2::IXMLDOMElementPtr& element, Color& outValue)
{
	BYTE A = outValue.GetA();
	BYTE R = outValue.GetR();
	BYTE G = outValue.GetG();
	BYTE B = outValue.GetB();
	getPrivateProfileInt(element, L"A", A);
	getPrivateProfileInt(element, L"R", R);
	getPrivateProfileInt(element, L"G", G);
	getPrivateProfileInt(element, L"B", B);
	outValue = Color(A, R, G, B);

	return S_OK;
}

inline HRESULT WINAPI setPrivateProfileColor(
	const MSXML2::IXMLDOMElementPtr& element, const Color& value)
{
	setPrivateProfileInt(element, L"A", value.GetA());
	setPrivateProfileInt(element, L"R", value.GetR());
	setPrivateProfileInt(element, L"G", value.GetG());
	setPrivateProfileInt(element, L"B", value.GetB());

	return S_OK;
}

//--------------------------------------------------------------------

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

//--------------------------------------------------------------------

extern COutProcessApp theApp;

//--------------------------------------------------------------------
