
#pragma once
#include "PageCom.h"
#include "PageRec.h"
#include "PageVAS.h"
#include "PageAN.h"

class CMySheet : public CPropertySheet
{
public:
	CMySheet();
	static CString GetWindowTitle();

private:
	DECLARE_DYNAMIC(CMySheet)
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	CPageCom m_pc;
	CPageRec m_pr;
	CPageVAS m_pv;
	CPageAN  m_pa;
};
