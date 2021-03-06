/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#ifndef _RECORDING_SOURCE_DLG_H
#define _RECORDING_SOURCE_DLG_H
#pragma once

#include "RecordingSourceItem.h"
#include "WasapiHelpers.h" //for DevicesArray

/////////////////////////////////////////////////////////////////////////////

#define WM_RECSOURCE_DLGCLOSED    WM_USER+IDD_RECORDING_SOURCE+0
#define WM_RECSOURCE_CHANGED      WM_USER+IDD_RECORDING_SOURCE+1
#define WM_RECSOURCE_ITEM_CLICKED WM_USER+IDD_RECORDING_SOURCE+2

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
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRecordingSourceDlg)
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnTimer(UINT_PTR nIDEvent); //device levels updating
	LRESULT OnRecodingSourceItemClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CRecordingSourceDlg(CWnd* pParent = NULL);
	void AdjustDialogCoordinates(CRect& windowRect) const;
	void UpdateDeviceArrays();

	void CreateRecordingSourceItems(const WasapiHelpers::DevicesArray& src);
	void UpdateRecordingSourceItems();
	void DeleteRecordingSourceItems();
	int  GetDefaultItemHeight() const;

	//CONFIG RELATED FUNCTIONS
	void LoadSelectedDevicesFromConfig();
	void SaveSelectedDevices();

private:
	std::vector<CRecordingSourceItem*> m_recordingSourceItems;
	WasapiHelpers::DevicesArray m_allDevices;
	WasapiHelpers::DevicesArray m_selectedDevices;
	bool m_multiSelection;
};

#endif // _RECORDING_SOURCE_DLG_H
