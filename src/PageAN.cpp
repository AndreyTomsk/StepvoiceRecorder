#include "stdafx.h"
#include "PageAN.h"
#include "Config.h"
#include "MainFrm_Helpers.h" //for FilterTemplate

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int TEMPLATE_LIMIT = 40;

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPageAN, CPropertyPage)

BEGIN_MESSAGE_MAP(CPageAN, CPropertyPage)
	ON_EN_CHANGE(IDE_NAMETEMPLATE, OnChangeNametemplate)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CPageAN::CPageAN()
	:CPropertyPage(CPageAN::IDD)
{
	m_BoldFont.CreateFont (-8, 0, 0, 0, FW_BOLD,  0, 0, 0, 0, 0, 0, 0, 0,
		"MS Sans Serif");
}
//---------------------------------------------------------------------------

CPageAN::~CPageAN()
{
}
//---------------------------------------------------------------------------

void CPageAN::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDE_NAMETEMPLATE, m_strTemplate);
}
//---------------------------------------------------------------------------

BOOL CPageAN::OnInitDialog() 
{
	m_defaultTemplate = _T("%b%d_%H%M");
	m_defaultTemplate = RegistryConfig::GetOption(_T("Autoname\\Default template"), m_defaultTemplate);
	m_strTemplate     = RegistryConfig::GetOption(_T("Autoname\\Current template"), m_defaultTemplate);

	CPropertyPage::OnInitDialog(); //internally calls DoDataExchange

	// пишем название шаблона по-умолчанию
	CString strDefText;
	strDefText.Format(_T("(default: %s):"), m_defaultTemplate);
	SetDlgItemText(IDC_AN_TDEFAULT, strDefText);
	
	// ограничиваем максимальную длину текста
	static_cast<CEdit*>(GetDlgItem(IDE_NAMETEMPLATE))->LimitText(TEMPLATE_LIMIT);
	OnChangeNametemplate();

	// изменяем курсор в группе template codes
	SetClassLong(GetDlgItem(IDC_STATIC0_0)->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_HAND2))); //вместо IDC_HAND
	
	return TRUE;
}
//---------------------------------------------------------------------------

void CPageAN::OnOK() 
{
	RegistryConfig::SetOption(_T("Autoname\\Current template"),
		Helpers::FilterTemplate(m_strTemplate));

	CPropertyPage::OnOK();
}
//---------------------------------------------------------------------------

void CPageAN::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// изменяем курсор в группе template codes на стардартный
	SetClassLong(GetDlgItem(IDC_STATIC0_0)->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ARROW))); //вместо IDC_HAND
}
//---------------------------------------------------------------------------

void CPageAN::OnChangeNametemplate() 
{
	UpdateData();
	m_strTemplate = Helpers::FilterTemplate(m_strTemplate);

	const CTime curTime = CTime::GetCurrentTime();
	CString exampleString = curTime.Format(m_strTemplate);
	if (exampleString.IsEmpty())
		exampleString = _T("Empty");

	exampleString += _T(".mp3");
	SetDlgItemText(IDC_AN_EXTEXT, exampleString);
}
//---------------------------------------------------------------------------

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
//---------------------------------------------------------------------------

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
			SetDlgItemText(IDE_NAMETEMPLATE, m_defaultTemplate);
			pETemplate->SetSel(0, m_defaultTemplate.GetLength(), false);
		}
	}

	return CPropertyPage::OnCommand(wParam, lParam);
}
//---------------------------------------------------------------------------

BOOL CPageAN::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());	
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
//---------------------------------------------------------------------------
