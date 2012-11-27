#ifndef __MALONE_TIMER_H__
#define __MALONE_TIMER_H__

#include <pthread.h>

namespace Malone
{
	
	class CTimerCallbacker
	{
	public:
		void OnTimer(int value, void *userdata);
	};
	
	struct SlotElement
	{
		int Loop;
		int Timeout;
		CTimerCallbacker *Callee;
		int Value;
		void *Userdata;
		int Timerid;
	};
	
	class CMaloneTimer
	{
	public:
		CMaloneTimer();
		void Start();
		void ProcTick();
		int Insert(int timeout, CTimerCallbacker *callee, int value, void *userdata);
	protected:
		int m_SlotNum;
		int m_Current;
		list<SlotElement> *m_SlotArray;
	};
}

#endif