// AfList.h: interface for the CAfList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFList_H__60E65C38_DB34_4529_B46A_01FE3BDBFE98__INCLUDED_)
#define AFX_AFList_H__60E65C38_DB34_4529_B46A_01FE3BDBFE98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
template <class T>
class CAfList  
{	typedef struct AfListItem
	{	T *pItem;
		AfListItem *pNext;
	} ListItem;
private:
	int m_nListSize;
	ListItem *m_begin, *m_end;
public:
	CAfList();
	~CAfList();
	
	void Push(T* pElem);
	void Pop();
	int Size() const {return m_nListSize;};
	T* GetTopItem() const {return (m_end) ? m_end->pItem : NULL;}; 
	T* operator[] (int num) const;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T>
CAfList<T>::CAfList() : m_begin(NULL), m_end(NULL), m_nListSize(0)
{
};

template <class T>
CAfList<T>::~CAfList()
{
	while(Size() != 0)
		Pop();
};

//////////////////////////////////////////////////////////////////////
template <class T>
void CAfList<T>::Push(T* pElem)
{
	// создаем новый элемент
	ListItem* pNewItem = new ListItem;
	pNewItem->pItem = pElem;
	pNewItem->pNext = NULL;

	// смещаем указатели
	if(Size() != 0)
		m_end->pNext = pNewItem;
	else
		m_begin = pNewItem;
	m_end = pNewItem;

	m_nListSize++;
};

//////////////////////////////////////////////////////////////////////
template <class T>
void CAfList<T>::Pop()
{
	if(Size() == 0)
		return;

	if(Size() > 1)
	{
		// находим предпоследний элемент
		int num = Size()-2;
		ListItem* pNewEnd = m_begin;
		while(num--)
			pNewEnd = pNewEnd->pNext;
		delete pNewEnd->pNext;
		pNewEnd->pNext = NULL;
		m_end = pNewEnd;
	}
	else
	{	delete m_begin;
		m_begin = NULL;
		m_end = NULL;
	}

	m_nListSize--;
};

//////////////////////////////////////////////////////////////////////
template <class T>
T* CAfList<T>::operator [] (int num) const
{
	if(num < 0 || num >= m_nListSize)
		return NULL;

	ListItem* pCurItem = m_begin;
	while(num--)
		pCurItem = pCurItem->pNext;

	return pCurItem->pItem;
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_AFList_H__60E65C38_DB34_4529_B46A_01FE3BDBFE98__INCLUDED_)
