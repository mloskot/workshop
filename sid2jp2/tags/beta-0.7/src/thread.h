//
// Copyright (C) 2002 Emad Barsoum. All rights reserved.
//
// Modified by Mateusz Loskot <mateusz@loskot.net>
//
#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include <windows.h>
#include <process.h>
#include <cassert>

namespace sid2jp2
{

template<typename T>
class Thread
{
public:

    //-- The constructor --
    Thread(T& thObject, void (T::*pfnOnRunning)(), int nPriority = THREAD_PRIORITY_NORMAL); 
    //-- Wait until the thread terminate, after this function you are sure that the thread is terminated.
    bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
    //-- Start the thread or recreate it, if it has been terminated before --
    bool Start(); 
    //-- Start the thread and return when it actualy start --
    bool StartAndWait();
    //-- Pause the thread --
    bool Pause();
    //-- Check if the thread is running or not.
    bool IsRunning() const;
    //-- check if the thread has been terminated or not.
    bool IsTerminated() const;
    //-- Check for the thread is suspend or not.
    bool IsSuspend() const;
    //-- Set thread priority
    void SetPriority(int nLevel);
    //-- Get thread priority
    int GetPriority() const;
    //-- Speed up thread execution - increase priority level
    void SpeedUp(); 
    //-- Slow down Thread execution - decrease priority level
    void SlowDown();
    //-- Terminate immediate the thread Unsafe.
    void Terminate();
    //-- Destructor
    virtual ~Thread()
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

}; // class Thread

/////////////////////////////////////////////////////////////////////////////

template<typename T> Thread<T>::Thread(T& thObject, void (T::*pfnOnRunning)(),
                                       int nPriority):m_thObject(thObject),
                                       m_pfnOnRunning(pfnOnRunning)
{
    m_hThread = NULL;
    m_dwThreadID = 0;
    m_bTerminate = true;
    m_bSuspend = true;
    m_bIsRunning = false;
    m_nInitPriority = nPriority;
    m_hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);      
}

template<typename T> bool Thread<T>::Start()
{
    if(m_bTerminate)
    {
        m_hThread = (HANDLE)_beginthreadex(NULL, 0, _ThreadProc,
                                           (LPVOID)this,
                                           0 /*CREATE_SUSPENDED*/,
                                           &m_dwThreadID); 

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

template<typename T> bool Thread<T>::StartAndWait()
{
    bool bRet = Start();
    if(bRet)
        ::WaitForSingleObject(m_hEvent,INFINITE);
    return bRet;
}

template<typename T> bool Thread<T>::Pause()
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

template<typename T> bool Thread<T>::IsRunning() const
{
    return m_bIsRunning;
}

template<typename T> bool Thread<T>::IsTerminated() const
{
    return m_bTerminate;
}

template<typename T> bool Thread<T>::IsSuspend() const
{
    return m_bSuspend;
}

template<typename T> void Thread<T>::Terminate()
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

template<typename T> void Thread<T>::Exit()
{
    DWORD dwExitCode;
    ::GetExitCodeThread(m_hThread,&dwExitCode);
    if (STILL_ACTIVE == dwExitCode)
    {
        _endthreadex(dwExitCode);
        ::ExiThread(dwExitCode);
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }
    m_bIsRunning = false;
    m_bTerminate = true;
}

template<typename T> bool Thread<T>::WaitUntilTerminate(DWORD dwMiliSec)
{
    if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread,dwMiliSec))
        return false;
    m_bIsRunning = false;
    m_bTerminate = true;
    return true;
}

template<typename T> void Thread<T>::SetPriority(int nLevel)
{
    ::SetThreadPriority(m_hThread, nLevel); 
}

template<typename T> int Thread<T>::GetPriority() const
{
    return ::GetThreadPriority(m_hThread);
}

template<typename T> void Thread<T>::SpeedUp()
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

template<typename T> void Thread<T>::SlowDown()
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

template<typename T> void Thread<T>::OnRunning()
{
    (m_thObject.*m_pfnOnRunning)();
        
    m_bIsRunning = false;
    m_bTerminate = true;
}

template<typename T> unsigned __stdcall Thread<T>::_ThreadProc(LPVOID lpParameter)
{
    Thread<T>* pThread = reinterpret_cast<Thread<T>*>(lpParameter);

    pThread->SetPriority(pThread->m_nInitPriority);
    pThread->m_bIsRunning = true;
    ::SetEvent(pThread->m_hEvent);
    pThread->OnRunning();

    return 0;
}

} // namespace sid2jp2

#endif  // THREAD_H_INCLUDED