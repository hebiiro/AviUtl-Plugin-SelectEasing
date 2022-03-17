#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Windows ヘッダーから使用されていない部分を除外します。
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 一部の CString コンストラクターは明示的です。
#define _AFX_ALL_WARNINGS   // 一般的で無視しても安全な MFC の警告メッセージの一部の非表示を解除します。
#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分
#include <afxdisp.h>        // MFC オートメーション クラス
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC の Internet Explorer 4 コモン コントロール サポート
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcontrolbars.h>     // MFC におけるリボンとコントロール バーのサポート

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#include <tchar.h>
#include <strsafe.h>

#include <vector>
#include <memory>
