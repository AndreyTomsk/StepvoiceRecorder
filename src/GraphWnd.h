/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_)
#define AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_

#include <bass.h>
#include "struct.h"

///////////////////////////////////////////////////////////////////////////////
class CGraphWnd : public CWnd
{
public:
	enum DisplayMode
	{
		E_DISPLAY_PEAKS,
		E_DISPLAY_PEAKS_DB,
		E_DISPLAY_LINES,
		E_DISPLAY_NONE
	};
public:
	CGraphWnd();
	~CGraphWnd();

	void SetMaxpeaks(bool bEnabled);
	bool GetMaxpeaks() const;

	// Visualization type (wave, peak meter, etc.)
	bool SetDisplayMode(DisplayMode a_new_mode);
	int  GetDisplayMode() const;

	void ShowVASMark(double fThreshold);
	void HideVASMark();

	// Start/stop visualization (a_stream_handle - handle to bass Stream)
	bool StartUpdate(HSTREAM a_stream_handle);
	void StopUpdate();
	
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
	double GetMaxPeakdB(char* pSndBuf, int nBufSize, int nChannel);
	int    GetPeakLevel(int nChannel);

	void Update(char* pSndBuf, int nBufSize);
	void Clear();
	void ClearWindow();
	void ResetMaxpeakMarks();
	void DrawVASMark();
	void DrawPeaks();
	void DrawLines();

	/// Max. possible peak value according to a sample type (8bit,16bit, float)
	const int  GetMaxPeakValue() const;
	
	static void CALLBACK UpdateVisualization(UINT a_timer_id, UINT a_msg,
		DWORD_PTR a_user_ptr, DWORD_PTR a_dw1, DWORD_PTR a_dw2);

private:
	CSize   m_wndsize;
	CPen    m_greenPen;
	CBrush  m_bkbrush;

	CDC     m_memDC;
	CDC     m_peakDC;
	CDC     m_skinDC;

	CBitmap m_bmp;
	CBitmap m_bgbmp[4];	// background bitmaps
	CBitmap m_pmbmp[3];	// peak bitmaps

	bool m_bShowVASMark;	// VAS mark display flag
	bool m_bMaxpeaks;

	double m_max_peaks[2];
	double m_old_peaks[2];
	double m_vas_marks[2];

	DWORD m_timer_id;
	DisplayMode m_display_mode;
	HSTREAM m_stream_handle;
	CRITICAL_SECTION m_sync_object;
};

///////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_)
