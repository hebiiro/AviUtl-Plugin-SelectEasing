#include "pch.h"
#include "InProcess.h"

//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
FILTER_DLL g_filter =
{
//	FILTER_FLAG_NO_CONFIG |
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_DISP_FILTER |
	FILTER_FLAG_EX_INFORMATION,
	0, 0,
	theApp.m_name,
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL,
	func_proc,
	func_init,
	func_exit,
	NULL,
	func_WndProc,
	NULL, NULL,
	NULL,
	NULL,
	theApp.m_information,
	NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL,
};

//---------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable(void)
{
	return &g_filter;
}

//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------

BOOL func_init(FILTER *fp)
{
	MY_TRACE(_T("func_init()\n"));

	return theApp.init(fp);
}

//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit(FILTER *fp)
{
	MY_TRACE(_T("func_exit()\n"));

	return theApp.exit(fp);
}

//---------------------------------------------------------------------
//		フィルタ関数
//---------------------------------------------------------------------
BOOL func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	MY_TRACE(_T("func_proc()\n"));

	return theApp.proc(fp, fpip);
}

//---------------------------------------------------------------------
//		WndProc
//---------------------------------------------------------------------
BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, void *editp, FILTER *fp)
{
	return theApp.WndProc(hwnd, message, wParam, lParam, editp, fp);
}

//---------------------------------------------------------------------
//		DllMain
//---------------------------------------------------------------------
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	return theApp.dllMain(instance, reason, reserved);
}
