/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_)
#define AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"

/////////////////////////////////////////////////////////////////////////////
class CGraphWnd : public CWnd
{
protected:
	CSize		m_wndsize;
	CPen		m_greenPen;
	CBrush		m_bkbrush;
	CRect		m_wndRect;
	
	CDC			m_memDC;
	CDC			m_peakDC;
	CDC			m_skinDC;
	
	CBitmap		m_bmp;
	CBitmap		m_bgbmp[4];	// битмапы фона окна
	CBitmap		m_pmbmp[3];	// битмапы пикового индикатора

	SOUND_INFO  m_si;
	float		m_kX, m_kY;		// коэффициенты масштабирования
	int			m_nSelector;	// выбор графика

	double		m_fVASThreshold;// уровень VAS
	bool		m_bShowVASMark;	// флаг вывода индикатора VAS

	int			m_maxpeakMarkL;
	int			m_maxpeakMarkR;
	
protected:
	void DrawPeaks(char* pSndBuf, int nBufSize, int nChannel, int nMode);
	void DrawLines(char* pSndBuf, int nBufSize, int nChannel, int nMode);
	int  GetPeakLevel(char* pSndBuf, int nBufSize, int nChannel);
public:
	void SetMaxpeaks(bool bEnabled);
	bool GetMaxpeaks();
	void SetGraphType(int nGraphType);
	int  GetGraphType();
	CGraphWnd();
	~CGraphWnd();
	void Update(char* pSndBuf, int nBufSize);
	void Clear();
	void SetConfig(SOUND_INFO* psi);
	double GetMaxPeakdB(char* pSndBuf, int nBufSize, int nChannel);
	void ShowVASMark(double fThreshold);
	void HideVASMark();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	void DrawVASMark();
	//{{AFX_MSG(CGraphWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGraphMaxpeaks();
	//}}AFX_MSG
	afx_msg void OnGraphMenu(UINT nID);
	afx_msg void OnUpdateGraphMenu(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
private:
	bool m_bMaxpeaks;
	void ResetMaxpeakMarks();
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_)
