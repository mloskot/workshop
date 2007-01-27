//
// Copyrights Emad Barsoum 2002. All rights reserved.
//
#include "stdafx.h"

#include <process.h>
#include "thread.h"

namespace thread
{

CThread::CThread(int nPriority)
{
    m_hThread = NULL;
    m_dwThreadID = 0;
    m_bTerminate = true;
    m_bSuspend = true;
    m_bIsRunning = false;
    m_nInitPriority = nPriority;
    m_hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);      
}

bool CThread::Start()
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

bool CThread::StartAndWait()
{
    bool bRet = Start();
    if(bRet)
        ::WaitForSingleObject(m_hEvent,INFINITE);

    return bRet;
}

bool CThread::Pause()
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

bool CThread::IsRunning()
{
    return m_bIsRunning;
}

bool CThread::IsTerminated()
{
    return m_bTerminate;
}

bool CThread::IsSuspend()
{
    return m_bSuspend;
}

void CThread::Terminate()
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

void CThread::Exit()
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

bool CThread::WaitUntilTerminate(DWORD dwMiliSec)
{
    if(WaitForSingleObject(m_hThread,dwMiliSec) == WAIT_TIMEOUT)
        return false;
    m_bIsRunning = false;
    m_bTerminate = true;
    return true;
}

void CThread::SetPriority(int nLevel)
{
    ::SetThreadPriority(m_hThread,nLevel); 
}

int CThread::GetPriority()
{
    return ::GetThreadPriority(m_hThread);
}

void CThread::SpeedUp()
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

void CThread::SlowDown()
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

unsigned __stdcall CThread::_ThreadProc(LPVOID lpParameter)
{
    CThread* pThread = reinterpret_cast<CThread*>(lpParameter);

    pThread->SetPriority(pThread->m_nInitPriority);
    pThread->m_bIsRunning = true;
    ::SetEvent(pThread->m_hEvent);
    pThread->OnInitInstance();
    pThread->OnRunning();
    return pThread->OnExitInstance();
}

} // namespace thread