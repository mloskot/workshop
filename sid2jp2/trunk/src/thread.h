//
// Copyrights Emad Barsoum 2002. All rights reserved.
//
// Those are two standard threads class, that encapsulate
// all the thread operation.
//
#ifndef THREAD__H_INCLUDED
#define THREAD__H_INCLUDED

#include <windows.h>
#include <process.h>
#include <cassert>

namespace thread
{

class CThread
{
public:
    //-- The constructor --
    CThread(int nPriority = THREAD_PRIORITY_NORMAL); 
    //-- Wait until the thread terminate, after this function you are sure that the thread is terminated.
    bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
    //-- Start the thread or recreate it, if it has been terminated before --
    bool Start(); 
    //-- Start the thread and return when it actualy start --
    bool StartAndWait();
    //-- Pause the thread --
    bool Pause();
    //-- Check if the thread is running or not.
    bool IsRunning();
    //-- check if the thread has been terminated or not.
    bool IsTerminated();
    //-- Check for the thread is suspend or not.
    bool IsSuspend();
    //-- Set thread priority
    void SetPriority(int nLevel);
    //-- Get thread priority
    int GetPriority();
    //-- Speed up thread execution - increase priority level
    void SpeedUp(); 
    //-- Slow down Thread execution - decrease priority level
    void SlowDown();
    //-- Terminate immediate the thread Unsafe.
    void Terminate();

protected:
    //-- put the initialization code here.
    virtual void OnInitInstance(){}
    //-- put the main code of the thread here.
    virtual void OnRunning() = 0; //-- Must be overloaded --
    //-- put the cleanup code here.
    virtual DWORD OnExitInstance(){return 0;}
    //-- Exit the thread safety.
    void Exit(); 
    //-- Thread function --
    static unsigned __stdcall _ThreadProc(LPVOID lpParameter);
    //-- Destructor --
    virtual ~CThread()
    {
        ::CloseHandle(m_hEvent);
    }

protected:
    HANDLE m_hThread, m_hEvent; //-- Thread and Event handle --
    int m_nInitPriority;
    unsigned int m_dwThreadID; //-- Contain the thread ID --
    //-- Variable to know the state of the thread terminated or suspend or Running -- 
    bool m_bTerminate, m_bSuspend, m_bIsRunning;

}; // class CThread

/////////////////////////////////////////////////////////////////////////////

template<typename T>
class TThread
{
public:
    //-- The constructor --
    TThread(T& thObject, void (T::*pfnOnRunning)(), int nPriority = THREAD_PRIORITY_NORMAL); 
    //-- Wait until the thread terminate, after this function you are sure that the thread is terminated.
    bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
    //-- Start the thread or recreate it, if it has been terminated before --
    bool Start(); 
    //-- Start the thread and return when it actualy start --
    bool StartAndWait();
    //-- Pause the thread --
    bool Pause();
    //-- Check if the thread is running or not.
    bool IsRunning();
    //-- check if the thread has been terminated or not.
    bool IsTerminated();
    //-- Check for the thread is suspend or not.
    bool IsSuspend();
    //-- Set thread priority
    void SetPriority(int nLevel);
    //-- Get thread priority
    int GetPriority();
    //-- Speed up thread execution - increase priority level
    void SpeedUp(); 
    //-- Slow down Thread execution - decrease priority level
    void SlowDown();
    //-- Terminate immediate the thread Unsafe.
    void Terminate();
    //-- Destructor
    virtual ~TThread()
    {
        ::CloseHandle(m_hEvent);
    }
protected:
    //-- Thread function --
    static unsigned __stdcall _ThreadProc(LPVOID lpParameter);
    //-- Exit the thread safety.
    void Exit(); 
    //-- Call the running member function --
    inline void OnRunning();

protected:
    T& m_thObject; //-- The thread object --
    void (T::*m_pfnOnRunning)();
    HANDLE m_hThread, m_hEvent; //-- Thread and Event handle --
    int m_nInitPriority;
    unsigned int m_dwThreadID; //-- Contain the thread ID --
    //-- Variable to know the state of the thread terminated or suspend or Running -- 
    bool m_bTerminate, m_bSuspend, m_bIsRunning;

}; // class TThread

/////////////////////////////////////////////////////////////////////////////

template<typename T> TThread<T>::TThread(T& thObject,void (T::*pfnOnRunning)(), int nPriority):m_thObject(thObject),m_pfnOnRunning(pfnOnRunning)
{
    m_hThread = NULL;
    m_dwThreadID = 0;
    m_bTerminate = true;
    m_bSuspend = true;
    m_bIsRunning = false;
    m_nInitPriority = nPriority;
    m_hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);      
}

template<typename T> bool TThread<T>::Start()
{
    if(m_bTerminate)
    {
        m_hThread = (HANDLE)_beginthreadex(NULL,0,_ThreadProc,(LPVOID)this,0/* CREATE_SUSPENDED*/,&m_dwThreadID); 

        m_bTerminate = false;
        m_bSuspend = false;

        if(m_hThread != 0)
            return true;
        return false;
    }
    else if(m_bSuspend)
    {
        DWORD nRet = ::ResumeThread(m_hThread);
        if(nRet == 0xFFFFFFFF)
            return false;
        m_bSuspend = false;
        return true;
    }
    return true;
}

template<typename T> bool TThread<T>::StartAndWait()
{
    bool bRet = Start();
    if(bRet)
        ::WaitForSingleObject(m_hEvent,INFINITE);
    return bRet;
}

template<typename T> bool TThread<T>::Pause()
{
    if(m_bTerminate)
        return false;
    if(m_bSuspend)
        return true;
    DWORD nRet = ::SuspendThread(m_hThread);
    if(nRet == 0xFFFFFFFF)
        return false;
    m_bSuspend = true;
    return true;
}

template<typename T> bool TThread<T>::IsRunning()
{
    return m_bIsRunning;
}

template<typename T> bool TThread<T>::IsTerminated()
{
    return m_bTerminate;
}

template<typename T> bool TThread<T>::IsSuspend()
{
    return m_bSuspend;
}

template<typename T> void TThread<T>::Terminate()
{
    DWORD dwExitCode;
    ::GetExitCodeThread(m_hThread,&dwExitCode);
    if (STILL_ACTIVE == dwExitCode)
    {
        ::TerminateThread(m_hThread,dwExitCode);
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }
    m_bIsRunning = false;
    m_bTerminate = true;
}

template<typename T> void TThread<T>::Exit()
{
    DWORD dwExitCode;
    ::GetExitCodeThread(m_hThread,&dwExitCode);
    if (STILL_ACTIVE == dwExitCode)
    {
        _endthreadex(dwExitCode);
        ::ExitThread(dwExitCode);
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }
    m_bIsRunning = false;
    m_bTerminate = true;
}

template<typename T> bool TThread<T>::WaitUntilTerminate(DWORD dwMiliSec)
{
    if(WaitForSingleObject(m_hThread,dwMiliSec) == WAIT_TIMEOUT)
        return false;
    m_bIsRunning = false;
    m_bTerminate = true;
    return true;
}

template<typename T> void TThread<T>::SetPriority(int nLevel)
{
    ::SetThreadPriority(m_hThread,nLevel); 
}

template<typename T> int TThread<T>::GetPriority()
{
    return ::GetThreadPriority(m_hThread);
}

template<typename T> void TThread<T>::SpeedUp()
{
    int nOldLevel;
    int nNewLevel;

    nOldLevel = GetPriority();
    if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
        return;
    else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
        nNewLevel = THREAD_PRIORITY_TIME_CRITICAL;
    else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_HIGHEST;
    else if (THREAD_PRIORITY_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_NORMAL;
    else if (THREAD_PRIORITY_LOWEST == nOldLevel)
        nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
    else if (THREAD_PRIORITY_IDLE == nOldLevel)
        nNewLevel = THREAD_PRIORITY_LOWEST;

    SetPriority(nNewLevel);
} 

template<typename T> void TThread<T>::SlowDown()
{
    int nOldLevel;
    int nNewLevel;

    nOldLevel = GetPriority();
    if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_HIGHEST;
    else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
        nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_NORMAL;
    else if (THREAD_PRIORITY_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
    else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_LOWEST;
    else if (THREAD_PRIORITY_LOWEST == nOldLevel)
        nNewLevel = THREAD_PRIORITY_IDLE;
    else if (THREAD_PRIORITY_IDLE == nOldLevel)
        return;

    SetPriority(nNewLevel);
}

template<typename T> void TThread<T>::OnRunning()
{
    (m_thObject.*m_pfnOnRunning)();
}

template<typename T> unsigned __stdcall TThread<T>::_ThreadProc(LPVOID lpParameter)
{
    TThread<T>* pThread = reinterpret_cast<TThread<T>*>(lpParameter);

    pThread->SetPriority(pThread->m_nInitPriority);
    pThread->m_bIsRunning = true;
    ::SetEvent(pThread->m_hEvent);
    pThread->OnRunning();
    return 0;
}

} // namespace thread

#endif  // THREAD__H_INCLUDED