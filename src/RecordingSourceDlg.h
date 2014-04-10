#ifndef _RECORDING_SOURCE_DLG_H
#define _RECORDING_SOURCE_DLG_H
#pragma once

/////////////////////////////////////////////////////////////////////////////

#define WM_RECSOURCE_DLGCLOSED WM_USER+IDD_RECORDING_SOURCE+0
#define WM_RECSOURCE_CHANGED   WM_USER+IDD_RECORDING_SOURCE+1

/////////////////////////////////////////////////////////////////////////////
class CRecordingSourceDlg : public CDialog
{
public:
	/*
	static bool Create(CPoint& pos, CWnd* parentWindow);
	static const CRecordingSourceDlg* GetInstance();
	*/
	static void Execute(CWnd* pParent); //modeless dialog construction

// Dialog Data
	//{{AFX_DATA(CRecordingSourceDlg)
	enum { IDD = IDD_RECORDING_SOURCE };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordingSourceDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CRecordingSourceDlg(CWnd* pParent = NULL);

	// Generated message map functions
	//{{AFX_MSG(CRecordingSourceDlg)
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void InitWasapiDevicesList(CCheckListBox&);
private:
	CCheckListBox m_checkList;
};

#endif // _RECORDING_SOURCE_DLG_H
