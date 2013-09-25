
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

typedef unsigned int uint;

template <class T>
class PoolList;

template <class T>
class PoolElement
{
public:
    PoolElement(): m_Next(NULL), m_Prev(NULL), m_Boss(NULL)
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
	
	void SetBoss(PoolList<T> *boss)
	{
		m_Boss = boss;
	}
    
    PoolElement *Next() { return m_Next; }
    
    PoolElement *Prev() { return m_Prev; }
	
	PoolList<T> *GetBoss() { return m_Boss; }
    
    T *GetObject()
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
	
protected:
    PoolElement *m_Next;
    PoolElement *m_Prev;
	PoolList<T> *m_Boss;
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
		element->SetBoss(this);
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
		element->SetBoss(this);
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
		ret->SetBoss(NULL);
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
		ret->SetBoss(NULL);
		return ret;
	}
	
	// first find if the element is in the list only by check GetBoss()
	PoolElement<T> *QuickRemoveElement(PoolElement<T> *element)
	{
		if (NULL == element || this != element->GetBoss())
			return NULL;
		
		if (m_First == element)
		{
			return Popfront();
		}
		
		if (m_Last == element)
		{
			return Popback();
		}
		
		PoolElement<T> *prev = element->Prev();
		PoolElement<T> *next = element->Next();
		prev->NextTo(next);
		next->PrevTo(prev);
		element->NextTo(NULL);
		element->PrevTo(NULL);
		element->SetBoss(NULL);
		--m_Count;
		return element;
	}
	
	// first find if the element is in the list by iterate the list, then remove it if true
	PoolElement<T> *RemoveFromList(PoolElement<T> *element)
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
		element->SetBoss(NULL);
		--m_Count;
		return element;
	}
	
	PoolElement<T> *RemoveFromList(int index)
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
	
protected:
	PoolElement<T> *m_First;
	PoolElement<T> *m_Last;
	uint m_Count;
};

template <class T>
class MaloneMemPool
{
public:
    MaloneMemPool(uint maxsize): m_Pool(NULL), m_Maxsize(maxsize)
    {
        if (maxsize <= 0)
        {
            printf("MaloneMemPool size <= 0\n");
            //exit(1);
        }
        m_Pool = new PoolElement<T>[m_Maxsize];
        if (NULL == m_Pool)
        {
            printf("no mem\n");
            //exit(1);
        }
        
        for (uint i = 0; i < m_Maxsize; ++i)
        {
            m_Free.Pushback(&m_Pool[i]);
        }

    }
    
    ~MaloneMemPool() { delete [] m_Pool; }
	
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
	
    int Release(T *rel) 
    {
		PoolElement<T> *element = GetContainerElement(rel);
		if (NULL != element)
		{
			if (NULL != m_Used.QuickRemoveElement(element))
			//if (NULL != m_Used.RemoveFromList(element))
			{
				m_Free.Pushback(element);
                return 1;
			} 
#if 0
			if (m_Used.IsInList(element))
			{
				m_Used.RemoveFromList(element);
				m_Free.Pushback(element);
                return 1;
			}
#endif
		}
        return 0;
    }

    static void Test()
    {
		const int size = 5;
        MaloneMemPool<T> pool(size);
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

public:	        
    PoolElement<T> * GetPoolArray() { return m_Pool; }
	PoolList<T> * GetUsedList() { return &m_Used; }
	PoolList<T> * GetFreeList() { return &m_Free; }

    uint NumberUsed() { return m_Used.GetCount(); }
    //uint NumberFree() { return (m_Maxsize - m_Used.GetCount()); }
    uint NumberFree() { return m_Free.GetCount(); }
	uint GetMaxsize () { return m_Maxsize; }
    
protected:
    uint m_Maxsize;
	// pool is array, you can access it by index
    PoolElement<T> *m_Pool;
	// used and free are organized as list, you should only
	// access them by next
    PoolList<T> m_Used;
    PoolList<T> m_Free;
    //void *m_Buffer;
    
};
#define CHUNK 10

template <class T>
class CAutoMemPool
{
public:
	CAutoMemPool(uint size): m_Size(size), m_LastUsed(0)
	{
		memset(m_Array, 0, sizeof(m_Array));
	}

    ~CAutoMemPool()
    {
        for (uint i = 0; i < CHUNK; ++i)
        {
            if (NULL != m_Array[i])
                delete m_Array[i];
        }
    }

	T *Allocate()
	{
		for (int i = 0; i < CHUNK; ++i)
		{
			if (NULL == m_Array[i])
				m_Array[i] = new MaloneMemPool<T>(m_Size);
			if (NULL == m_Array[i])
				return NULL;
			T *data = m_Array[i]->Allocate();
			if (NULL != data)
			{
				m_LastUsed = max(m_LastUsed, i);
				return data;
			}
		}

		return NULL;
	}

    void Release(T *rel)
    {
        for (uint i = 0; i <= m_LastUsed; ++i)   
        {
            if (NULL == m_Array[i])
                return;
            if (m_Array[i]->Release(rel))
                return;
            continue;
        }
#if 0 
        for (uint i = 0; i < CHUNK; ++i)   
        {
            if (NULL == m_Array[i])
                continue;
            if (m_Array[i]->Release(rel))
                return;
            continue;
        }
#endif
    }

	// only release the last not used 
	int Shrink()
	{
		int effect = 0;
		for (int i = m_LastUsed; i > -1; --i)
		{
            //printf("shrink last %d\n", m_LastUsed);
			if (NULL == m_Array[i])
			{
				--m_LastUsed;
				if (m_LastUsed < 0)
					m_LastUsed = 0;
				continue;
			}
			if (m_Array[i]->GetMaxsize() == m_Array[i]->NumberFree() && 
				m_LastUsed == i)
			{
				delete m_Array[i];
				m_Array[i] = NULL;
				--m_LastUsed;
				if (m_LastUsed < 0)
					m_LastUsed = 0;
				++effect;
			}
			else
			{
				break;
			}
		}
		return effect;
#if 0
		int effect = 0;
		for (uint i = 0; i < CHUNK; ++i)    
        {   
			if (NULL == m_Array[i])
				continue;
			if (m_Array[i]->GetMaxsize() == m_Array[i]->NumberFree())
			{
				delete m_Array[i];
				m_Array[i] = NULL;
				++effect;
			}
            continue;
        }	
		return effect;
#endif
	}	

	uint SpaceFree()
	{
		uint total = 0;
		for (uint i = 0; i < CHUNK; ++i)
		{
			if (NULL != m_Array[i])
			{
				total += m_Array[i]->NumberFree();
			}
		}
		return total;
	}

	uint SpaceUsed()
	{
		uint total = 0;
		for (uint i = 0; i < CHUNK; ++i)
		{
			if (NULL != m_Array[i])
			{
				total += m_Array[i]->NumberUsed();
			}
		}
		return total;
	}
	
	uint SpaceMax()
	{
		return m_Size * CHUNK;
	}

protected:
	MaloneMemPool<T> *m_Array[CHUNK];
	uint m_Size;
	int m_LastUsed;
};

void Doit()
{
    const int LEN = 10000000;
    int** pi = new int*[LEN];
    memset(pi, 0, sizeof(pi));
    CAutoMemPool<int> pool(LEN / 10);
    for (int i = 0; i < LEN; ++i)
    {
        pi[i] = pool.Allocate();
        if (pi[i] == NULL)
            printf("NULL\n");
    }
    printf("%d, %d\n", pool.SpaceFree(), pool.SpaceUsed());
    //getchar();
    /*for (int i = LEN - 5000; i >= 0; --i)
    {
        pool.Release(pi[i]);
    }
    if (pool.Shrink())
        printf("shrink ok\n");
    printf("%d, %d\n", pool.SpaceFree(), pool.SpaceUsed());


    for (int i = LEN - 1; i >= 8000; --i)
    {
        pool.Release(pi[i]);
    }
    if (pool.Shrink())
        printf("shrink ok\n");
    printf("%d, %d\n", pool.SpaceFree(), pool.SpaceUsed());
*/
    for (int i = LEN - 1; i >= 0; --i)
        pool.Release(pi[i]);
    if (pool.Shrink())
        printf("shrink ok\n");
    printf("%d, %d\n", pool.SpaceFree(), pool.SpaceUsed());
	delete [] pi;
}

#if 1 
int main()
{
    Doit();
    //getchar();
#if 1 
    cout << "compileroffset " << compileroffset << endl;
    PoolElement<int> pi;
    printf("offset %i\n", PoolElement<int>::ObjectOffest());
    printf("offset %i\n", pi.ObjectOffest2());
    MaloneMemPool<int>::Test();
#endif
    return 0;
}
#endif

#endif

