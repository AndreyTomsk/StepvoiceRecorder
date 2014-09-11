#ifndef _RECORDING_SOURCE_DLG_H
#define _RECORDING_SOURCE_DLG_H
#pragma once

#include "RecordingSourceItem.h"
#include "WasapiHelpers.h" //for DevicesArray

/////////////////////////////////////////////////////////////////////////////

#define WM_RECSOURCE_DLGCLOSED WM_USER+IDD_RECORDING_SOURCE+0
#define WM_RECSOURCE_CHANGED   WM_USER+IDD_RECORDING_SOURCE+1

/////////////////////////////////////////////////////////////////////////////
class CRecordingSourceDlg : public CDialog
{
public:
	static CRecordingSourceDlg* GetInstance();

	WasapiHelpers::DevicesArray GetSelectedDevices() const;
	void SelectDevices(const WasapiHelpers::DevicesArray& src);
	void Execute(CPoint& pos);

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
	// Generated message map functions
	//{{AFX_MSG(CRecordingSourceDlg)
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDevicesListSelChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent); //device levels updating
	DECLARE_MESSAGE_MAP()

private:
	CRecordingSourceDlg(CWnd* pParent = NULL);
	void UpdateDevicesListBox(const WasapiHelpers::DevicesArray& devices, CCheckListBox& listBox);

private:
	CCheckListBox m_checkList;
	WasapiHelpers::DevicesArray m_allDevices;
	WasapiHelpers::DevicesArray m_selectedDevices;

	std::vector<CRecordingSourceItem*> m_recordingSourceItems;
};

#endif // _RECORDING_SOURCE_DLG_H
