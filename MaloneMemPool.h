
#ifndef __MALONE_MEM_POOL_H__
#define __MALONE_MEM_POOL_H__

#include <stdio.h>
#include <iostream>

using namespace std;

#ifdef __compiler_offsetof
#define OFFSETOF(TYPE, MEMBER) __compiler_offsetof(TYPE,MEMBER)
int compileroffset = 1;
#else
#define OFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
int compileroffset = 0;
#endif

template <class T>
class PoolElement
{
public:
    PoolElement(): m_Next(NULL), m_Prev(NULL)
    {
    }
    
    void NextTo(PoolElement *next) 
    {
        m_Next = next;
    }
    
    void PrevTo(PoolElement *prev)
    {
        m_Prev = prev;
    }
    
    PoolElement * Next() { return m_Next; }
    
    PoolElement * Prev() { return m_Prev; }
    
    T * GetObject()
    {
        return &m_Obj;
    }
    
    static int ObjectOffest()
    {
        size_t offset = OFFSETOF(PoolElement<T>, m_Obj);
        return offset;
    }

    int ObjectOffest2()
    {
        size_t offset = (size_t)((char *)&this->m_Obj - (char *)this);
        return offset;
    }

    PoolElement *m_Next;
    PoolElement *m_Prev;
    T m_Obj;
};

template <class T>
class PoolList
{
public:
	PoolList(): m_Count(0), m_First(NULL), m_Last(NULL)
	{
	}
	
	void Clear()
	{
		m_First = NULL;
		m_Last = NULL;
		m_Count = 0;
	}

    void PrintElement()
    {
        int i;
        for (i = 0; i < GetCount(); i++)
        {       
            PoolElement<T> *pe = GetElementByIndex(i);
            cout << *pe->GetObject() << ", ";
        } 
    }
	
	void Pushback(PoolElement<T> *element)
	{
		if (NULL == element)
			return;
		if (NULL == m_First)
		{
			m_First = element;
			m_Last = m_First;
			m_First->NextTo(NULL);
			m_First->PrevTo(NULL);
		}
		else
		{
			m_Last->NextTo(element);
			element->PrevTo(m_Last);
			m_Last = element;
			m_Last->NextTo(NULL);
		}
		++m_Count;
	}
	
	void Pushfront(PoolElement<T> *element)
	{
		if (NULL == element)
			return;
		if (NULL == m_First)
		{
			m_First = element;
			m_Last = m_First;
			m_First->NextTo(NULL);
			m_First->PrevTo(NULL);
		}
		else
		{
			m_First->PrevTo(element);
			element->NextTo(m_First);
			m_First = element;
			m_First->PrevTo(NULL);
		}
		++m_Count;
	}
	
	PoolElement<T> *Popback()
	{
		if (NULL == m_Last)
			return NULL;
		PoolElement<T> *ret;
		if (m_First == m_Last)
		{
			ret = m_First;
			m_First = NULL;
			m_Last = NULL;
			m_Count = 0;
		}
		else
		{
			ret = m_Last;
			m_Last = m_Last->Prev();
			m_Last->NextTo(NULL);
			--m_Count;
		}
		
		ret->NextTo(NULL);
		ret->PrevTo(NULL);
		return ret;
	}
	
	PoolElement<T> *Popfront()
	{
		if (NULL == m_Last)
			return NULL;
		PoolElement<T> *ret;
		if (m_First == m_Last)
		{
			ret = m_First;
			m_First = NULL;
			m_Last = NULL;
			m_Count = 0;
		}
		else
		{
			ret = m_First;
			m_First = m_First->Next();
			m_First->PrevTo(NULL);
			--m_Count;
		}
		
		ret->NextTo(NULL);
		ret->PrevTo(NULL);
		return ret;
	}
	
	PoolElement<T> * RemoveFromList(PoolElement<T> *element)
	{
		if (element == NULL)
			return NULL;
		if (!IsInList(element))
			return NULL;
		if (element == m_First)
			return Popfront();
		if (element == m_Last)
			return Popback();
		PoolElement<T> *prev = element->Prev();
		PoolElement<T> *next = element->Next();
		prev->NextTo(next);
		next->PrevTo(prev);
		element->NextTo(NULL);
		element->PrevTo(NULL);
		--m_Count;
		return element;
	}
	
	PoolElement<T> * RemoveFromList(int index)
	{
		PoolElement<T> *element = GetElementByIndex(index);
		return RemoveFromList(element);
	}
	
	int IsInList(PoolElement<T> *element)
	{
		if (element == NULL)
            return 0;
        PoolElement<T> * it = m_First;
        for (; it != NULL; it = it->Next())
        {
            if (it == element)
                return 1;
        }
        
        return 0;
	}
	
	int IndexOf(PoolElement<T> *element)
	{
		if (element == NULL)
            return 0;
        PoolElement<T> * it = m_First;
        for (int i = 0; it != NULL; ++i, it = it->Next())
        {
            if (it == element)
                return i;
        }
        
        return -1;
	}
	
	PoolElement<T> * GetElementByIndex(int index)
    {
        if (NULL == m_First)
            return NULL;
		if (index < 0 || index >= m_Count)
			return NULL;
			
        PoolElement<T> *it = m_First;
        for (int i = 0; i < index; ++i)
        {
            if (NULL != it)
                it = it->Next();
            else
            {
                return NULL;
            }
        } 
        return it;
    }
	
	int GetCount()
	{
		return m_Count;
	}
	
	int CalcCount()
    {
        if (NULL == m_First)
            return 0;
        PoolElement<T> *it = m_First;
        int count = 0;
        do
        {
            it = it->Next();
            ++count;
        } while (it != NULL);
        return count;
    } 
	
	PoolElement<T> * GetLastFromList()
    {
        if (NULL == m_First)
            return NULL;
		return m_Last;
    }  
	
	PoolElement<T> *m_First;
	PoolElement<T> *m_Last;
	int m_Count;
};

template <class T>
class CMaloneMemPool
{
public:
    CMaloneMemPool(int maxsize): m_Pool(NULL), m_Maxsize(maxsize)
    {
        if (maxsize <= 0)
        {
            printf("CMaloneMemPool size <= 0\n");
            //exit(1);
        }
        m_Pool = new PoolElement<T>[m_Maxsize];
        if (NULL == m_Pool)
        {
            printf("no mem\n");
            //exit(1);
        }
        
        for (int i = 0; i < m_Maxsize; ++i)
        {
            m_Free.Pushback(&m_Pool[i]);
        }

    }
    
    ~CMaloneMemPool() { delete [] m_Pool; }
	
    T * Allocate() 
    { 
        if (m_Free.GetCount() == 0)
            return NULL;
		
		PoolElement<T> *element = m_Free.Popfront();
		
		if (NULL != element)
        {
            m_Used.Pushback(element);
			return element->GetObject();
        }
		return NULL;
    }
	
    void Release(T *rel) 
    {
		PoolElement<T> *element = GetContainerElement(rel);
		if (NULL != element)
		{
			if (m_Used.IsInList(element))
			{
				m_Used.RemoveFromList(element);
				m_Free.Pushback(element);
			}
		}
    }

    static void Test()
    {
		const int size = 5;
        CMaloneMemPool<T> pool(size);
        int i = 0;
        T *array[size];
		for (i = 0; i < size; i++)
		{
			T *t = pool.Allocate();
			*t = i;
            array[i] = t;
		}

		PoolList<T> *li = pool.GetUsedList();
		PoolList<T> *li2 = pool.GetFreeList();
        cout << "used" << endl;
        li->PrintElement();
        cout << endl;
        cout << "free" << endl;
        li2->PrintElement();
        cout << endl;
        pool.Release(array[0]);
        cout << "used" << endl;
        li->PrintElement();
        cout << endl;
        cout << "free" << endl;
        li2->PrintElement();
        cout << endl;

        pool.Release(array[4]);
        cout << "used" << endl;
        li->PrintElement();
        cout << endl;
        cout << "free" << endl;
        li2->PrintElement();
        cout << endl;

        pool.Release(array[2]);
        cout << "used" << endl;
        li->PrintElement();
        cout << endl;
        cout << "free" << endl;
        li2->PrintElement();
        cout << endl;

        pool.Release(array[3]);
        cout << "used" << endl;
        li->PrintElement();
        cout << endl;
        cout << "free" << endl;
        li2->PrintElement();
        cout << endl;


        pool.Release(array[1]);
        cout << "used" << endl;
        li->PrintElement();
        cout << endl;
        cout << "free" << endl;
        li2->PrintElement();
        cout << endl;

    }
    // test only, the user only known they can get object T from mempool,
    // the implementation and structure of mempool is encapsulated
protected:

	
	// return the element that contain the object
	PoolElement<T> * GetContainerElement(T *obj)
	{
		size_t position = (size_t)obj;
        // now we get the mem position of Element<T> which contain obj
        position -= PoolElement<T>::ObjectOffest();
		size_t arrayposition = (size_t)m_Pool;
        size_t elementsize = sizeof(PoolElement<T>);
        
        if (position < arrayposition || 
            position > arrayposition + (m_Maxsize - 1) * elementsize)
        {
            return NULL;
        }
        
        if ((position - arrayposition) % elementsize != 0)
        {
            return NULL;
        }
		
		int index = (position - arrayposition) / elementsize;
        if (index < 0 || index >= m_Maxsize)
            return NULL;
		
		return &m_Pool[index];
	}
	        
    PoolElement<T> * GetPoolArray() { return m_Pool; }
	PoolList<T> * GetUsedList() { return &m_Used; }
	PoolList<T> * GetFreeList() { return &m_Free; }
    
protected:
    int m_Maxsize;
	// pool is array, you can access it by index
    PoolElement<T> *m_Pool;
	// used and free are organized as list, you should only
	// access them by next
    PoolList<T> m_Used;
    PoolList<T> m_Free;
    //void *m_Buffer;
    
};

#if 0
int main()
{
    cout << "compileroffset " << compileroffset << endl;
    PoolElement<int> pi;
    printf("offset %i\n", PoolElement<int>::ObjectOffest());
    printf("offset %i\n", pi.ObjectOffest2());
    CMaloneMemPool<int>::Test();
    return 0;
}
#endif

#endif

