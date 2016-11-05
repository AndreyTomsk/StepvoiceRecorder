/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
class CSliderVol : public CSliderCtrl
{
public:
	CSliderVol();
	virtual ~CSliderVol();

public:
	bool IsDragging() const { return m_is_dragging; }
	bool IsUpdating() const { return m_is_updating; }
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSliderVol)
	//}}AFX_VIRTUAL

	//volumeLevel - in range 0..1.
	void  SetVolume(float volumeLevel);
	float GetVolume() const;

	void ShowAutomaticVolume(bool autoVolume);

protected:
	//{{AFX_MSG(CSliderVol)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCustomDraw(NMHDR* pNotifyStruct, LRESULT* result);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	friend class CSliderUpdateLock;
	bool m_is_dragging;
	bool m_is_updating;
	bool m_showAutomaticVolume;

	CBrush m_brush;
	CPen m_pen;
};
//------------------------------------------------------------------------------

class CSliderUpdateLock
{
public:
	CSliderUpdateLock(CSliderVol& slider) : m_slider(slider) { m_slider.m_is_updating = true; }
	~CSliderUpdateLock() { m_slider.m_is_updating = false; }
private:
	CSliderVol& m_slider;
};
