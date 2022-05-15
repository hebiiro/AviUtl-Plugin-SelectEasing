﻿#include "pch.h"
#include "OutProcess.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT CMainFrame::saveSettings()
{
	MY_TRACE(_T("CMainFrame::saveSettings()\n"));

	// 設定ファイルを読み込めていない場合は、保存処理をするとファイルの中の設定値がすべて初期値に戻ってしまう。
	// よって、フラグを確認して設定ファイルを読み込めていない場合は保存処理をしないようにする。
	if (!m_isSettingsFileLoaded)
		return S_FALSE;

	try
	{
		// ドキュメントを作成する。
		MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

		// ドキュメントエレメントを作成する。
		MSXML2::IXMLDOMElementPtr element = appendElement(document, document, L"settings");

		// <easing> を作成する。
		saveEasing(element);

		// <ease> を作成する。
		m_easeWindow.saveEase(element);

		return saveXMLDocument(document, m_fileUpdateChecker.getFilePath(), L"UTF-16");
	}
	catch (_com_error& e)
	{
		MY_TRACE(_T("%s\n"), e.ErrorMessage());
		return e.Error();
	}
}

// <easing> を作成する。
HRESULT CMainFrame::saveEasing(const MSXML2::IXMLDOMElementPtr& element)
{
	MY_TRACE(_T("CMainFrame::saveEasing()\n"));

	// <easing> を作成する。
	MSXML2::IXMLDOMElementPtr easingElement = appendElement(element, L"easing");

	// <easing> のアトリビュートを作成する。
	setPrivateProfileInt(easingElement, L"imageVersion", m_imageVersion);
	setPrivateProfileBool(easingElement, L"clamp", m_clamp);
	setPrivateProfileString(easingElement, L"horz", m_horz);
	setPrivateProfileString(easingElement, L"vert", m_vert);
	setPrivateProfileInt(easingElement, L"alpha", m_alpha);
	setPrivateProfileInt(easingElement, L"scale", m_scale);

	{
		MSXML2::IXMLDOMElementPtr selectedElement = appendElement(easingElement, L"selected");

		setPrivateProfileColor(selectedElement, m_selectedColor);
	}

	{
		MSXML2::IXMLDOMElementPtr hotElement = appendElement(easingElement, L"hot");

		setPrivateProfileColor(hotElement, m_hotColor);
	}

	return S_OK;
}
