/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_)
#define AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_

#include <bass.h>
#include "struct.h"

///////////////////////////////////////////////////////////////////////////////
class CGraphWnd : public CWnd
{
	void ClearWindow();
public:
	CGraphWnd();
	~CGraphWnd();

	void SetMaxpeaks(bool bEnabled);
	bool GetMaxpeaks() const;

	//void SetGraphType(int nGraphType);
	//int  GetGraphType();

	void Update(char* pSndBuf, int nBufSize);
	void Clear();

	double GetMaxPeakdB(char* pSndBuf, int nBufSize, int nChannel);

	void ShowVASMark(double fThreshold);
	void HideVASMark();

	/**
	 *  Start visualization.
	 *  @param a_stream_handle - handle to bass Stream;
	 *  @param a_display_mode - display mode (-1 - keep current).
	 */
	bool StartUpdate(HSTREAM a_stream_handle);
	/// Stop visualization
	void StopUpdate();
	/// Select visualization type
	bool SetDisplayMode(int a_new_display_mode);
	/// Get display mode (wave, peak meter, etc.)
	const int GetDisplayMode() const;

public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphWnd)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
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
	void ResetMaxpeakMarks();
	void DrawVASMark();
	void DrawPeaks(int nMode);
	void DrawLines();
	int  GetPeakLevel(int nChannel);
	/// Max. possible peak value according to a sample type (8bit,16bit, float)
	const int  GetMaxPeakValue() const;
	
	static void CALLBACK UpdateVisualization(UINT a_timer_id, UINT a_msg,
		DWORD_PTR a_user_ptr, DWORD_PTR a_dw1, DWORD_PTR a_dw2);

private:
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

	bool m_bShowVASMark;	// флаг вывода индикатора VAS
	bool m_bMaxpeaks;

	double m_max_peaks[2];
	double m_old_peaks[2];
	double m_vas_marks[2];

private:
	DWORD m_timer_id;
	unsigned int m_display_mode;
	HSTREAM m_stream_handle;
	CRITICAL_SECTION m_sync_object;
};

///////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_)
