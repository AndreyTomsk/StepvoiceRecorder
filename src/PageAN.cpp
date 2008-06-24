// PageAN.cpp : implementation file
//

#include "stdafx.h"
#include "mp3_recorder.h"
#include "PageAN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TEMPLATE_LIMIT 40

/////////////////////////////////////////////////////////////////////////////
// CPageAN property page

IMPLEMENT_DYNCREATE(CPageAN, CPropertyPage)

CPageAN::CPageAN() : CPropertyPage(CPageAN::IDD)
{
	m_pconfig = NULL;

	m_BoldFont.CreateFont (-8, 0, 0, 0, FW_BOLD,  0, 0, 0, 0, 0, 0, 0, 0,
		"MS Sans Serif");

	//{{AFX_DATA_INIT(CPageAN)
	m_strTemplate = _T("");
	//}}AFX_DATA_INIT
}

CPageAN::~CPageAN()
{
}

void CPageAN::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageAN)
	DDX_Text(pDX, IDE_NAMETEMPLATE, m_strTemplate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPageAN, CPropertyPage)
	//{{AFX_MSG_MAP(CPageAN)
	ON_EN_CHANGE(IDE_NAMETEMPLATE, OnChangeNametemplate)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CPageAN::SetConfig(CONF_DIAL_AN *pconfig)
{
	m_pconfig = pconfig;
	m_strTemplate = m_pconfig->strAutoName;
}

void CPageAN::OnChangeNametemplate() 
{
	CWnd* l_wnd_template = (CWnd *)GetDlgItem(IDE_NAMETEMPLATE);
	CWnd* l_wnd_result = (CWnd *)GetDlgItem(IDC_AN_EXTEXT);

	CString l_format_string;
	l_wnd_template->GetWindowText(l_format_string);
	l_format_string = FilterTemplate(l_format_string);

	CTime l_time = CTime::GetCurrentTime();
	CString l_result_string = l_time.Format(l_format_string);

	if (l_result_string.IsEmpty())
	{
		l_result_string = _T("Empty");
	}

	l_result_string += _T(".mp3");
	l_wnd_result->SetWindowText(l_result_string);
}

CString CPageAN::FilterTemplate(CString a_template)
{
	const CString ALLOWED_CHARS(_T("BbdHjMmSYy%"));
	CString l_template = a_template;
	CString l_char_str;

	l_template.TrimRight('%'); // important! the algorithm below using this

	int l_pos = 0;
	while (l_pos < l_template.GetLength())
	{
		if (l_template.Mid(l_pos, 1) != _T("%"))
		{
			l_pos++;	// ordinary char, jump to next symbol
			continue;
		}
		
		l_char_str = l_template.Mid(l_pos + 1, 1);
		if (ALLOWED_CHARS.Find(l_char_str) == -1)
		{
			l_template.Delete(l_pos, 2);
			continue;	// checking the updated string from same position
		}

		l_pos += 2;		// char is correct, jump over 2 symbols
	}

	return l_template;
}

void CPageAN::OnOK() 
{
	if(m_pconfig)
	{
		m_pconfig->strAutoName = FilterTemplate(m_strTemplate);
	}
	
	CPropertyPage::OnOK();
}

BOOL CPageAN::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// пишем название шаблона по-умолчанию
	CString strDefText = "";
	strDefText.Format("(default: %s):", m_pconfig->strTDefault);
	SetDlgItemText(IDC_AN_TDEFAULT, strDefText);
	
	// ограничиваем максимальную длину текста
	CEdit* pETemplate = (CEdit *)GetDlgItem(IDE_NAMETEMPLATE);
	pETemplate->LimitText(TEMPLATE_LIMIT);
	OnChangeNametemplate();

	// изменяем курсор в группе template codes
	CStatic* pStatic = (CStatic *)GetDlgItem(IDC_STATIC0_0);
	SetClassLong(pStatic->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_HAND2))); //вместо IDC_HAND
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CPageAN::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	int nID[] = {
		IDC_STATIC0_0, IDC_STATIC1_0, IDC_STATIC2_0, IDC_STATIC3_0,
		IDC_STATIC4_0, IDC_STATIC5_0, IDC_STATIC6_0, IDC_STATIC7_0,
		IDC_STATIC8_0, IDC_STATIC9_0};

	for(int i=0; i < sizeof(nID)/sizeof(int); i++) {
		if (pWnd->m_hWnd == GetDlgItem(nID[i])->m_hWnd)
			pDC->SelectObject (&m_BoldFont);
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CPageAN::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	int nCode = HIWORD(wParam);

	if(nCode == BN_CLICKED || nCode == BN_DOUBLECLICKED)
	{
		switch(nID) {
		case IDC_STATIC0_1: nID = IDC_STATIC0_0; break;
		case IDC_STATIC1_1: nID = IDC_STATIC1_0; break;
		case IDC_STATIC2_1: nID = IDC_STATIC2_0; break;
		case IDC_STATIC3_1: nID = IDC_STATIC3_0; break;
		case IDC_STATIC4_1: nID = IDC_STATIC4_0; break;
		case IDC_STATIC5_1: nID = IDC_STATIC5_0; break;
		case IDC_STATIC6_1: nID = IDC_STATIC6_0; break;
		case IDC_STATIC7_1: nID = IDC_STATIC7_0; break;
		case IDC_STATIC8_1: nID = IDC_STATIC8_0; break;
		case IDC_STATIC9_1: nID = IDC_STATIC9_0; break;
		}

		CEdit* pETemplate = (CEdit *)GetDlgItem(IDE_NAMETEMPLATE);

		// удаляем выделенный текст (если есть)
		pETemplate->ReplaceSel("", true);

		if(nID != IDC_AN_TDEFAULT)
		{
			// добавляем код в позицию курсора
			CString strTemplate, strCode;
			GetDlgItemText(nID, strCode);
			GetDlgItemText(IDE_NAMETEMPLATE, strTemplate);

			int nCharNum = pETemplate->CharFromPos(pETemplate->GetCaretPos());
			strTemplate = strTemplate.Left(nCharNum) + strCode +
				strTemplate.Right(strTemplate.GetLength() - nCharNum);
			if(strTemplate.GetLength() <= TEMPLATE_LIMIT)
				SetDlgItemText(IDE_NAMETEMPLATE, strTemplate);

			// перемещаем курсор на позицию после добавленного кода
			nCharNum += 2; // т.к. код всегда состоит из двух символов
			pETemplate->SetSel(nCharNum, nCharNum, false);
		}
		else
		{	// очищаем весь шаблон
			CString strTemplate = m_pconfig->strTDefault;
			SetDlgItemText(IDE_NAMETEMPLATE, strTemplate);
			pETemplate->SetSel(0, strTemplate.GetLength(), false);
		}
	}

	return CPropertyPage::OnCommand(wParam, lParam);
}

void CPageAN::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// изменяем курсор в группе template codes на стардартный
	CStatic* pStatic = (CStatic *)GetDlgItem(IDC_STATIC0_0);
	SetClassLong(pStatic->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ARROW))); //вместо IDC_HAND
}

BOOL CPageAN::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());
	
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
