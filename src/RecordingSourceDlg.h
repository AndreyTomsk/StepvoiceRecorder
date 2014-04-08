#ifndef _RECORDING_SOURCE_DLG_H
#define _RECORDING_SOURCE_DLG_H

#if _MSC_VER > 1000
#pragma once
#endif

/////////////////////////////////////////////////////////////////////////////

#define WM_RECSOURCE_DLGCLOSED WM_USER+IDD_RECORDING_SOURCE+0
#define WM_RECSOURCE_CHANGED   WM_USER+IDD_RECORDING_SOURCE+1

/////////////////////////////////////////////////////////////////////////////
class CRecordingSourceDlg : public CDialog
{
public:
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
  afx_msg void OnBnClickedButton1();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#endif // _RECORDING_SOURCE_DLG_H
