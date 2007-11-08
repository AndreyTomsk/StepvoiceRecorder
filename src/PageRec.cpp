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
	//{{AFX_MSG_MAP(CPageRec)
	ON_BN_CLICKED(IDC_AUTOFREQ, OnAutofreq)
	ON_WM_HSCROLL()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int BITRATE[11] = {8, 16, 24, 32, 48, 64, 96, 128, 160, 192, 256};
int FREQ[4]		= {8000, 11025, 22050, 44100};

//===========================================================================
CPageRec::CPageRec() : CPropertyPage(CPageRec::IDD)
{
	//{{AFX_DATA_INIT(CPageRec)
	m_bAutoFreq		= FALSE;
	m_nSampleFreq	= 2;
	m_nStereo		= 0;
	//}}AFX_DATA_INIT

	pconfig			= NULL;
	m_nBitrate		= 4;
}

//===========================================================================
void CPageRec::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CPageRec)
	DDX_Check(pDX, IDC_AUTOFREQ,	m_bAutoFreq);
	DDX_Radio(pDX, IDC_SAMPLEFREQ,	m_nSampleFreq);
	DDX_Radio(pDX, IDC_MONO,		m_nStereo);
	//}}AFX_DATA_MAP
}

//===========================================================================
BOOL CPageRec::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// настраиваем параметры и положение слайдера выбора битрейта
	CSliderCtrl* brs = (CSliderCtrl *)GetDlgItem(IDC_SLIDER1);
	brs->SetRange(0, 10);
	brs->SetLineSize(1);
	brs->SetPageSize(1);
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION,
		m_nBitrate), (long)brs->GetSafeHwnd());

	// меняем вид выбора частоты в зависимости от флага Autofreq
	OnAutofreq();
	
	return TRUE;
}

//===========================================================================
void CPageRec::OnOK() 
{
	CSliderCtrl* brs = (CSliderCtrl *)GetDlgItem(IDC_SLIDER1);

	// меняем MP3 настройки
	ASSERT(pconfig);
	pconfig->nBitrate	= BITRATE[brs->GetPos()];
	pconfig->nFreq		= FREQ[m_nSampleFreq];
	pconfig->nStereo	= m_nStereo;
	pconfig->bAutoFreq	= m_bAutoFreq;

	CPropertyPage::OnOK();
}

//===========================================================================
void CPageRec::OnAutofreq() 
{
// Обработка выбора чекбокса Autofreq.
// В зависимости от него меняем состояние рамки "Выбор частоты" и всех
// (или только части) групбоксов, находящихся в ней.

	// получаем указатели слайдера битрейта и рамки выбора частоты
	CSliderCtrl* brs = (CSliderCtrl *)GetDlgItem(IDC_SLIDER1);
	CStatic* group	 = (CStatic *)GetDlgItem(IDC_GROUP_FREQ);

	int nChecked= IsDlgButtonChecked(IDC_AUTOFREQ);
	if(nChecked)
	{
		EnableFreqSel(false, false);		// делаем все групбоксы disabled
		//group->ModifyStyle(0, WS_DISABLED);	// делаем рамку тоже disabled :)
		SetFreq(BITRATE[brs->GetPos()]);	// автоматич. устанавливаем частоту
	}
	else
	{
		EnableFreqSel(BITRATE[brs->GetPos()]);	// меняем состояние отдельных groupbox
		//group->ModifyStyle(WS_DISABLED, 0);	// убираем disabled c рамки
	}
	group->Invalidate(true);				// перерисовываем рамку
}

//===========================================================================
void CPageRec::EnableFreqSel(int nBitBITRATE)
{
	if(nBitBITRATE == 32) {
		EnableFreqSel(true, true);
		return;
	}
	// determine which radio button checked
	int nID[] = {IDC_SAMPLEFREQ, IDC_11KHZ, IDC_22KHZ, IDC_44KHZ};
	for(int i = 0; i<4; i++)
		if(IsDlgButtonChecked(nID[i])) break;

	if(nBitBITRATE < 32) {
		EnableFreqSel(true, false);	// Enable MPEG2 bitBITRATEs
		if(i > 1) SetFreq(24);
	}else {
		EnableFreqSel(false, true);	// Enable MPEG1 bitBITRATEs
		if(i < 2) SetFreq(64);
	}
}

//===========================================================================
void CPageRec::EnableFreqSel(bool bmpg2freq, bool bmpg1freq)
{
	CButton* btn;
	int nID[]	= {IDC_SAMPLEFREQ, IDC_11KHZ, IDC_22KHZ, IDC_44KHZ};

	for(int i = 0; i<2; i++) {
		btn = (CButton *)GetDlgItem(nID[i]);
		if(bmpg2freq) btn->ModifyStyle(WS_DISABLED, 0);
		else		  btn->ModifyStyle(0, WS_DISABLED);
		btn->Invalidate(false);
	}
	for(i = 2; i<4; i++) {
		btn = (CButton *)GetDlgItem(nID[i]);
		if(bmpg1freq) btn->ModifyStyle(WS_DISABLED, 0);
		else		  btn->ModifyStyle(0, WS_DISABLED);
		btn->Invalidate(false);
	}
}

//===========================================================================
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
	CButton* btn = (CButton *)GetDlgItem(nID[i]);
	for(int j = 0; j<4; j++)
		if(j == i) CheckDlgButton(nID[i], BST_CHECKED);
		else CheckDlgButton(nID[j], BST_UNCHECKED);
}

//===========================================================================
void CPageRec::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	char  szText[100];
	char* szQuality[] = {/*8*/  "Phone-",   /*16*/ "Phone", /*24*/ "MW-EU",
						 /*32*/ "AM radio", /*48*/ "MW-US", /*64*/ "Voice",
						 /*96*/ "FM radio", /*128*/"Tape",  /*160*/"HiFi",
						 /*192*/"near CD",       /*256*/"CD"};

	// получаем указатели слайдера и строки описания битрейта
	CStatic*     sbrd = (CStatic *)GetDlgItem(IDC_BITRATE_DESCR);
	CSliderCtrl* brs  = (CSliderCtrl *)pScrollBar;

	// обрабатываем сообщение от слайдера
	int curpos	= brs->GetPos();
	switch (nSBCode) {
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	}
	brs->SetPos(curpos);

	// отображаем строку, поясняющую качество записи при таком битрейте
	float fMinMb = (float)BITRATE[curpos]/8 * 60 / 1024; // переводим в Mb
	sprintf(szText, "1 minute record - %3.2f Mb, 1 hour - %3.1f Mb (%s quality);",
		fMinMb, fMinMb*60, szQuality[curpos]);
	sbrd->SetWindowText(szText);

	// если флаг авточастоты включен...
	if(IsDlgButtonChecked(IDC_AUTOFREQ))
	{
		// ... устанавливаем новую частоту
		SetFreq(BITRATE[curpos]);
	}
	else
	{	// ... иначе изменяем доступность отдельных полей
		EnableFreqSel(BITRATE[curpos]);
	}
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

//===========================================================================
void CPageRec::SetConfig(CONF_DIAL_MP3* pconf_dial_mp3)
{
// Получить положение элементов диалога.
// Вход: указатель на структуру настроек MP3.

	// сохраняем указатель (пригодится при сохранении настроек)
	pconfig	= pconf_dial_mp3;

	// находим порядковый номер частоты (исп. для группы чекбоксов)
	m_nSampleFreq = int(std::find(FREQ, FREQ+4, pconfig->nFreq) - FREQ);
	m_nSampleFreq = (m_nSampleFreq < 4) ? m_nSampleFreq : 2;

	m_nBitrate    = int(std::find(BITRATE, BITRATE+11, pconfig->nBitrate) - BITRATE);
	m_nBitrate    = (m_nBitrate < 11) ? m_nBitrate : 4;

	m_nStereo	  = pconfig->nStereo;
	m_bAutoFreq	  = pconfig->bAutoFreq;

}

//===========================================================================
BOOL CPageRec::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTEXTHELP;
	return CPropertyPage::PreCreateWindow(cs);
}

BOOL CPageRec::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
