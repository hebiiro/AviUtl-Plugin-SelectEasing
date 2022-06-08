#include "pch.h"
#include "InProcess.h"
#include "InProcess_Hook.h"

//--------------------------------------------------------------------

IMPLEMENT_HOOK_PROC_NULL(BOOL, CDECL, GetParamSmallExternal, (int index))
{
	__asm { MOV theApp.m_trackOffsets, ESI }
	theApp.m_trackIndex = index;

	MY_TRACE(_T("GetParamSmallExternal(%d)\n"), index);

	int oldValue = theApp.m_trackTable[index];
	BOOL retValue = true_GetParamSmallExternal(index);
	int newValue = theApp.m_trackTable[index];

	if (!retValue && newValue != oldValue)
	{
		MY_TRACE(_T("値を復元します\n"));

		// 値を確定せずにダイアログを閉じたにも関わらず、
		// 値が変更されているので元の値を復元する。
		theApp.update(oldValue);
	}

	theApp.m_trackOffsets = 0;
	theApp.m_trackIndex = 0;

	return retValue;
}

//--------------------------------------------------------------------
