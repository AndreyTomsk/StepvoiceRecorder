/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_)
#define AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_

#include <bass.h>
#include "struct.h"
#include "SyncObjects.h"

///////////////////////////////////////////////////////////////////////////////
// The main idea here is to work only with float data. And having one format
//   of data, we could simplify the work.
// Peaks callback function returns peak value from [0, 1] interval.
// Lines callback function asks to fill a float array of certain size with
//   [0, 1] values. It returns the actual size filled (could be 0).

typedef float (*PEAKS_CALLBACK)(int a_channel);
typedef int   (*LINES_CALLBACK)(int a_channel, float* a_buffer, int a_size);

///////////////////////////////////////////////////////////////////////////////
class CGraphWnd : public CWnd
{
public:
	enum DisplayMode
	{
		E_DISPLAY_PEAKS,	// Linear peaks
		E_DISPLAY_PEAKS_DB, // Logarithmic peaks
		E_DISPLAY_LINES,    // Line meter
		E_DISPLAY_NONE
	};
public:
	CGraphWnd();
	~CGraphWnd();

	void ShowMaxpeaks(bool a_enabled);
	bool MaxpeaksVisible() const;

	void ShowVASMark(bool a_enabled, double a_threshold = 0);
	bool VASMarkVisible() const;

	bool SetDisplayMode(DisplayMode a_new_mode);
	int  GetDisplayMode() const;

	bool StartUpdate(PEAKS_CALLBACK a_peaks_func, LINES_CALLBACK a_lines_func);
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
	afx_msg void OnGraphMonitoring();
	//}}AFX_MSG
	afx_msg void OnGraphMenu(UINT nID);
	afx_msg void OnUpdateGraphMenu(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

private:
	float GetPeakLevel(int a_channel) const;
	int GetLinesLevel(int a_channel, float* a_buffer, int a_size);

	void Update(char* pSndBuf, int nBufSize);
	void Clear();
	void ClearWindow();
	void ResetMaxpeakMarks();
	void DrawVASMark();
	void DrawPeaks();
	void DrawLines();

	static void CALLBACK UpdateVisualization(UINT a_timer_id, UINT a_msg,
		DWORD_PTR a_user_ptr, DWORD_PTR a_dw1, DWORD_PTR a_dw2);

private:
	PEAKS_CALLBACK m_peaks_func;
	LINES_CALLBACK m_lines_func;

	CSize   m_wndsize;
	CPen    m_greenPen;
	CBrush  m_bkbrush;

	CDC     m_memDC;
	CDC     m_peakDC;
	CDC     m_skinDC;

	CBitmap m_bmp;
	CBitmap m_bgbmp[4];  // background bitmaps
	CBitmap m_pmbmp[3];  // peak bitmaps

	bool m_bShowVASMark; // VAS mark display flag
	bool m_bMaxpeaks;    // Maximum peaks flag
	bool m_bMonitoring;  // Input monitoring flag

	double m_max_peaks[2];
	double m_old_peaks[2];
	double m_vas_marks[2];

	DWORD m_timer_id;
	DisplayMode m_display_mode;
	CMyCriticalSection m_sync_object;
};

///////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_GRAPHWND_H__0750808D_BAB1_4092_A8C2_3D8EAB4D7D3D__INCLUDED_)
