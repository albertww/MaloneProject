#ifndef __MALONE_TIMER_H__
#define __MALONE_TIMER_H__

#include <pthread.h>

namespace Malone
{
	
	class CTimerCallbacker
	{
	public:
		void OnTimer(int id, void *param1, void *param2);
	};
	
	struct SlotElement
	{
		int Loop;
		int Timeout;
		CTimerCallbacker *CallBackFunc;
		void *FirstParam;
		void *SecondParam;
	};
	
	class CMaloneTimer
	{
	public:
		CMaloneTimer();
		void Start();
		void ProcTick();
		int Insert(int timeout, CTimerCallbacker *caller, void *param1, void *param2);
	protected:
		int m_SlotNum;
		int m_Current;
		list<SlotElement> *m_SlotArray;
	};
}

#endif