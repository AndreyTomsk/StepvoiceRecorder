/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "PageRec.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPageRec, CPropertyPage)

BEGIN_MESSAGE_MAP(CPageRec, CPropertyPage)
	ON_BN_CLICKED(IDC_AUTOFREQ, OnAutofreq)
	ON_WM_HSCROLL()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

const int BITRATE[] = {8, 16, 24, 32, 48, 64, 96, 128, 160, 192, 256};
const int FREQ[] = {8000, 11025, 22050, 44100};

/////////////////////////////////////////////////////////////////////////////

CPageRec::CPageRec()
	:CPropertyPage(CPageRec::IDD)
	,m_bitrateIndex(7)
	,m_frequencyIndex(3)
	,m_autoFrequency(FALSE)
	,m_isStereo(1)
{
}
//---------------------------------------------------------------------------

void CPageRec::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_AUTOFREQ,	m_autoFrequency);
	DDX_Radio(pDX, IDC_SAMPLEFREQ,	m_frequencyIndex);
	DDX_Radio(pDX, IDC_MONO,		m_isStereo);
}
//---------------------------------------------------------------------------

BOOL CPageRec::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTEXTHELP;
	return CPropertyPage::PreCreateWindow(cs);
}
//---------------------------------------------------------------------------

BOOL CPageRec::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
//---------------------------------------------------------------------------

BOOL CPageRec::OnInitDialog() 
{
	const int configBitrate = RegistryConfig::GetOption(_T("File types\\MP3\\Bitrate"), 128);
	const int configFreq = RegistryConfig::GetOption(_T("File types\\MP3\\Freq"), 44100);
	m_autoFrequency = RegistryConfig::GetOption(_T("File types\\MP3\\AutoFreq"), 1);
	m_isStereo = RegistryConfig::GetOption(_T("File types\\MP3\\Stereo"), 1);

	m_bitrateIndex = int(std::find(BITRATE, BITRATE+11, configBitrate) - BITRATE);
	m_bitrateIndex = (m_bitrateIndex >= 0 && m_bitrateIndex < 11) ? m_bitrateIndex : 4;

	m_frequencyIndex = int(std::find(FREQ, FREQ+4, configFreq) - FREQ);
	m_frequencyIndex = (m_frequencyIndex >= 0 && m_frequencyIndex < 4) ? m_frequencyIndex : 2;

	CPropertyPage::OnInitDialog(); //internally calls DoDataExchange

	CSliderCtrl* bitrateSlider = static_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDER1));
	bitrateSlider->SetRange(0, 10);
	bitrateSlider->SetLineSize(1);
	bitrateSlider->SetPageSize(1);
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION,
		m_bitrateIndex), (long)bitrateSlider->GetSafeHwnd());

	OnAutofreq();
	return TRUE;
}
//---------------------------------------------------------------------------

void CPageRec::OnOK() 
{
	CSliderCtrl* bitrateSlider = static_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDER1));

	RegistryConfig::SetOption(_T("File types\\MP3\\Bitrate"), BITRATE[bitrateSlider->GetPos()]);
	RegistryConfig::SetOption(_T("File types\\MP3\\Freq"),    FREQ[m_frequencyIndex]);
	RegistryConfig::SetOption(_T("File types\\MP3\\Stereo"),  m_isStereo);
	RegistryConfig::SetOption(_T("File types\\MP3\\AutoFreq"),m_autoFrequency);

	CPropertyPage::OnOK();
}
//---------------------------------------------------------------------------

void CPageRec::OnAutofreq() 
{
// ¬ зависимости от него мен€ем состо€ние рамки "¬ыбор частоты" и всех
// (или только части) групбоксов, наход€щихс€ в ней.

	// получаем указатели слайдера битрейта и рамки выбора частоты
	CSliderCtrl* bitrateSlider = static_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDER1));
	const int configBitrate = BITRATE[bitrateSlider->GetPos()];

	if (IsDlgButtonChecked(IDC_AUTOFREQ))
	{
		EnableFreqSel(false, false);	// делаем все групбоксы disabled
		SetFreq(configBitrate);			// автоматич. устанавливаем частоту
	}
	else
	{
		EnableFreqSel(configBitrate);	// мен€ем состо€ние отдельных groupbox
	}

	CStatic* frequencyGroup = static_cast<CStatic *>(GetDlgItem(IDC_GROUP_FREQ));
	frequencyGroup->Invalidate(true);
}
//---------------------------------------------------------------------------

void CPageRec::EnableFreqSel(int nBitBITRATE)
{
	int i;

	if (nBitBITRATE == 32)
	{
		EnableFreqSel(true, true);
		return;
	}
	// determine which radio button checked
	const int nID[] = {IDC_SAMPLEFREQ, IDC_11KHZ, IDC_22KHZ, IDC_44KHZ};
	for(i = 0; i < 4; i++)
	{
		if (IsDlgButtonChecked(nID[i]))
			break;
	}

	((CWnd*)GetDlgItem(IDC_22KHZ))->EnableWindow(TRUE);
	if (nBitBITRATE < 32)
	{
		EnableFreqSel(true, false);	// Enable MPEG2 bitBITRATEs
		if (i > 1)
			SetFreq(24);
	}
	else if (nBitBITRATE < 160)
	{
		EnableFreqSel(false, true);	// Enable MPEG1 bitBITRATEs
		if (i < 2)
			SetFreq(64);
	}
	else
	{	// When a bitrate is 160 kbps and more, then disable the IDC_22KHZ
		EnableFreqSel(false, true);	// Enable MPEG1 bitBITRATEs
		((CWnd*)GetDlgItem(IDC_22KHZ))->EnableWindow(FALSE);
		if (i < 3)
			SetFreq(128);
	}
}
//---------------------------------------------------------------------------

void CPageRec::EnableFreqSel(bool bmpg2freq, bool bmpg1freq)
{
	int i;
	CButton* btn;
	const int nID[]	= {IDC_SAMPLEFREQ, IDC_11KHZ, IDC_22KHZ, IDC_44KHZ};

	for (i = 0; i < 2; i++)
	{
		btn = (CButton *)GetDlgItem(nID[i]);
		if(bmpg2freq) btn->ModifyStyle(WS_DISABLED, 0);
		else		  btn->ModifyStyle(0, WS_DISABLED);
		btn->Invalidate(false);
	}
	for (i = 2; i < 4; i++)
	{
		btn = (CButton *)GetDlgItem(nID[i]);
		if(bmpg1freq) btn->ModifyStyle(WS_DISABLED, 0);
		else		  btn->ModifyStyle(0, WS_DISABLED);
		btn->Invalidate(false);
	}
}
//---------------------------------------------------------------------------

void CPageRec::SetFreq(int nBitBITRATE)
{
	int i = 0;
	int nID[] = {IDC_SAMPLEFREQ, IDC_11KHZ, IDC_22KHZ, IDC_44KHZ};

	if(nBitBITRATE < 16) i = 0;
	else
		if(nBitBITRATE < 32) i = 1;
		else
			if(nBitBITRATE <= 64) i = 2;
			else i = 3;
	//CButton* btn = (CButton *)GetDlgItem(nID[i]);
	for(int j = 0; j<4; j++)
		if(j == i) CheckDlgButton(nID[i], BST_CHECKED);
		else CheckDlgButton(nID[j], BST_UNCHECKED);
}
//---------------------------------------------------------------------------

void CPageRec::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	const CString strQuality[] = {
		 /*8*/_T("Phone-"),   /*16*/_T("Phone"), /*24*/_T("MW-EU"),
		/*32*/_T("AM radio"), /*48*/_T("MW-US"), /*64*/_T("Voice"),
		/*96*/_T("FM radio"), /*128*/_T("Tape"), /*160*/_T("HiFi"),
		/*192*/_T("near CD"), /*256*/_T("CD")
	};

	// получаем указатели слайдера и строки описани€ битрейта
	CSliderCtrl* brs  = (CSliderCtrl *)pScrollBar;

	// обрабатываем сообщение от слайдера
	int curpos = brs->GetPos();
	switch (nSBCode) {
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	}
	brs->SetPos(curpos);

	const float fMinMb = (float)BITRATE[curpos]/8 * 60 / 1024; // переводим в Mb
	CString bitrateInfo;
	bitrateInfo.Format(_T("1 minute record - %3.2f Mb, 1 hour - %3.1f Mb (%s quality);"),
		fMinMb, fMinMb*60, strQuality[curpos]);

	SetDlgItemText(IDC_BITRATE_DESCR, bitrateInfo);

	// если флаг авточастоты включен...
	if(IsDlgButtonChecked(IDC_AUTOFREQ))
	{
		// ... устанавливаем новую частоту
		SetFreq(BITRATE[curpos]);
	}
	else
	{	// ... иначе измен€ем доступность отдельных полей
		EnableFreqSel(BITRATE[curpos]);
	}
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}
//---------------------------------------------------------------------------
