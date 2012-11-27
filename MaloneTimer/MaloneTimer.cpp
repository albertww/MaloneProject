#include "MaloneTimer.h"

namespace Malone
{
	CMaloneTimer::CMaloneTimer(): m_SlotNum(60), m_Current(0), m_SlotArray(NULL)
	{
		m_SlotArray = new list<SlotElement>[m_SlotNum];
	}

	void CMaloneTimer::ProcTick()
	{
		++m_Current;
		if (m_Current >= m_SlotNum)
			m_Current = m_Current % m_SlotNum;
		if (m_SlotArray[m_Current].size() > 0)
		{
			list<SlotElement>::iterator it = m_SlotArray[m_Current].begin();
			while (it != m_SlotArray[m_Current].end())
			{
				if (--(it->Loop) < 0)
				{
					// do action
					// then remove from list
					m_SlotArray[m_Current].erase(it++);
				}
			}
		}
	}
	
	int CMaloneTimer::Insert(int timeout, CTimerCallbacker *callee, int value, void *userdata)
	{
		if (timeout <= 0)
			return 0;
		SlotElement se;
		se.Timeout = timeout;
		se.Callee = callee;
		se.Value = value;
		se.Userdata = userdata;
		int loop = timeout / m_SlotNum;
		int nextSlot = (m_Current + timeout) % m_SlotNum;
		se.Loop = loop;
		m_SlotArray[nextSlot].push_back(se);
		return 1;
	}
	
}