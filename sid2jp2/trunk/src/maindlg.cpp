// $Id$
/////////////////////////////////////////////////////////////////////////////
// MainDlg.cpp : implementation of the MainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"
#include "maindlg.h"
#include "error.h"
#include "filesystem.h"
#include "translator.h"
#include "mrsidimageinfo.h"
// gdal
#include <gdal.h>
#include <cpl_error.h>
#include <cpl_string.h>
// std
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <utility>
// boost
#include <boost/lexical_cast.hpp>
// winstl
#include <winstl/findfile_sequence.hpp>

using namespace sid2jp2;

MainDlg::MainDlg() :
    m_mode(eModeNone),
    m_driver(NULL),
    m_worker(m_translator, &Translator::Run)
{
    // idle
}

/////////////////////////////////////////////////////////////////////////////
// Windows Message Handlers
/////////////////////////////////////////////////////////////////////////////

BOOL MainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL MainDlg::OnIdle()
{
    UIUpdateChildWindows();

	return FALSE;
}

LRESULT MainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	// center the dialog on the screen
	CenterWindow();

	// Load icons
	HICON hIcon = WTL::AtlLoadIconImage(MAKEINTRESOURCE(IDR_MAINFRAME), LR_DEFAULTCOLOR,
                    ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
    ATLASSERT(NULL != hIcon);
	SetIcon(hIcon, TRUE);

    HICON hIconSmall = WTL::AtlLoadIconImage(MAKEINTRESOURCE(IDR_MAINFRAME), LR_DEFAULTCOLOR,
                        ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	ATLASSERT(NULL != hIconSmall);
    SetIcon(hIconSmall, FALSE);

    // Hook up variables to controls.
    BOOL bExchanged = DoDataExchange(DDX_LOAD);
	ATLASSERT(TRUE == bExchanged);

    // Initialize UI
    UISetStateReady();

    // Initialize data
    m_mode = eModeSingle;
    m_ratioMode = eInputFileRatio;

    // Initialize GDAL environment
    GDALAllRegister();
    GDALSetCacheMax(eCacheMax);

	// Register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	return TRUE;
}

void MainDlg::OnSysCommand(UINT nCode, WTL::CPoint /*point*/)
{
    SetMsgHandled(FALSE);

    switch (nCode)
    {
    case SC_CLOSE:
        SendMessage(WM_CLOSE);
        SetMsgHandled(TRUE);
        break;
    }
}

LRESULT MainDlg::OnStart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    try
    {

        BOOL ret = DoDataExchange(DDX_SAVE);
        ATLASSERT(TRUE == ret);

        // Common message buffor
        ATL::CString msg;
        UIResetProgressBar();

        //
        // Initialize output driver
        //
        ret = InitializeGDALDriver();
        if (!ret)
            return 0;

        //
        // Collect translation options
        //

        char** copyOptions = NULL;
        //copyOptions = ::CSLSetNameValue(copyOptions, "TARGET", targetRatio.c_str());
        copyOptions = ::CSLSetNameValue(copyOptions, "LARGE_OK", "YES");

        if (eModeSingle == m_mode && m_files.size() > 1)
        {
            msg.Format(_T("There are too many files selected for single mode processing!!"));
            MessageBox(msg, _T("Error!"), MB_OK | MB_ICONERROR);
            return 0;
        }

        //
        // Generate list of pairs of input/output MrSID images
        //
        // TODO: mloskot - replace hardcoded strings with resources
        m_ctlProgressInfo.SetWindowText(_T("Generating input files list..."));

        // Prepare dataset collection
        ClearDatasetList();

        if (m_pathInput.IsEmpty() || m_pathOutput.IsEmpty())
        {
            m_ctlProgressInfo.SetWindowText(_T("Ready!"));
            msg.Format(_T("Please, select source and target dataset!"));
            MessageBox(msg, _T("Error!"), MB_OK | MB_ICONSTOP);
            return 0;
        }

        std::string inputPath = CT2A(m_pathInput);
        std::string outputPath = CT2A(m_pathOutput);

        if (eModeSingle == m_mode)
        {
            // Display processing directory
            ATL::CPath baseInputPath(m_pathInput);
            ret = baseInputPath.RemoveFileSpec();
            ATLASSERT(TRUE == ret);
            msg.Format(_T("Directory: %s"), baseInputPath);
            m_ctlProgressFileInfo.SetWindowText(msg);

            if (!inputPath.empty() && !outputPath.empty())
            {
                m_files.push_back(std::make_pair(inputPath, outputPath));
            }
        }
        else if (eModeBatch == m_mode)
        {
            m_ctlProgressInfo.SetWindowText(_T("Generating list of files..."));

            std::vector<std::string> tmpList;
            fs::generate_file_list(inputPath, "*.sid", tmpList);

            // Rebuild basic list to colletion of input/output paths
            fs::generate_dataset_list(inputPath, tmpList, outputPath, m_files, ".jp2", false);

        }
        else if (eModeBatchRecursive == m_mode)
        {
            m_ctlProgressInfo.SetWindowText(_T("Generating list of files in recursive mode..."));

            std::vector<std::string> tmpList;
			fs::generate_file_list(inputPath, "*.sid", tmpList);
            fs::generate_file_list_recurse(inputPath, "*.sid", tmpList);

            // Rebuild basic list to colletion of input/output paths
            fs::generate_dataset_list(inputPath, tmpList, outputPath, m_files, ".jp2", true);

        }
        else
        {
            // NEVER SHOULD GET HERE
            ATLASSERT("[sid2jp2] Unknown processing mode");
        }

        //
        // Generated and assign Target Compression Rations per image
        //        
        if (eInputFileRatio == m_ratioMode)
        {
            for (std::vector<dataset_t>::const_iterator it = m_files.begin();
                 it != m_files.end(); ++it)
            {
                std::string const& inputFile = it->first;
                
                MrSidImageInfo sidInfo(inputFile);
                m_ratios[inputFile] = sidInfo.GetCompressionRatio();
            }
        }
        else
        {
            // Assign common Target Compression Ratio to all images

            ATL::CString buf;
            m_ctlRatioBox.GetWindowText(buf);
            int ratio = eRatioMin;

            try
            {
                std::string tmp = CT2A(buf);
                ratio = boost::lexical_cast<int>(tmp);
            }
            catch(boost::bad_lexical_cast const& e)
            {
                std::string msg("Invalid value of compression ratio: ");
                throw std::runtime_error(msg + e.what());
            }

            for (std::vector<dataset_t>::const_iterator it = m_files.begin();
                 it != m_files.end(); ++it)
            {
                std::string const& inputFile = it->first;    
                m_ratios[inputFile] = ratio;
            }
        }

        // Check post-conditions
        assert(m_files.size() == m_ratios.size());

        //
        // Set busy state
        //
        UISetStateBusy();

        //
        // Start processing collection of files
        //
        m_translator.Configure(m_hWnd, m_driver, copyOptions, m_files, m_ratios);
        m_worker.StartAndWait();

    }
    catch (std::exception const& e)
    {
        ATL::CString msg(CA2T(e.what()));
        MessageBox(msg, _T("Error!"), MB_OK | MB_ICONSTOP);
    }

    return 0;
}

LRESULT MainDlg::OnStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    BOOL bRet = StopTranslation();    

    return 0;
}

LRESULT MainDlg::OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    // If translation is in progress, ask for abort.
    if (IsTranslating())
    {
        if (!StopTranslation())
        {
            // Close action aborted
            return 0;
        }
    }

    CloseDialog(wID);
	return 0;
}

LRESULT MainDlg::OnAppAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	AboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT MainDlg::OnAppHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    MessageBox(_T("Simple manual will be here available"), _T("Help!"), MB_OK);
	return 0;
}

LRESULT MainDlg::OnModeChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if(BN_CLICKED == wNotifyCode)
    {
        if (IsDlgButtonChecked(IDC_MODE_SINGLE))
        {
            m_mode = eModeSingle;
            UIEnable(IDC_MODE_BATCH_RECURSIVE, false);
        }
        else if (IsDlgButtonChecked(IDC_MODE_BATCH))
        {
            m_mode = eModeBatch;
            
            UIEnable(IDC_MODE_BATCH_RECURSIVE, true);
        }

        // Reset input/output paths
        UISetStatePathBoxes(false);
    }

    return 0;
}

LRESULT MainDlg::OnModeRecursiveChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if (IsDlgButtonChecked(IDC_MODE_BATCH_RECURSIVE))
        m_mode = eModeBatchRecursive;
    else
        m_mode = eModeBatch;

    return 0;
}

LRESULT MainDlg::OnRatioFromInputChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    // Button checked, Target Compression Ratio is set from input
    BOOL bActivateInput = IsDlgButtonChecked(IDC_OPT_RATIO_FROM_INPUT);
    UISetStateOptions(bActivateInput);

    if (bActivateInput)
        m_ratioMode = eInputFileRatio;
    else
        m_ratioMode = eUserRatio;

    return 0;
}
LRESULT MainDlg::OnRatioSpinChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LPNMHDR pNMHDR = NULL;
    LPNMUPDOWN pNMUD = NULL;

    pNMHDR = reinterpret_cast<LPNMHDR>(lParam);
    ATLASSERT(NULL != pNMHDR);

    switch(pNMHDR->code)
    {
    case UDN_DELTAPOS:
        pNMUD = reinterpret_cast<LPNMUPDOWN>(lParam);
        ATLASSERT(NULL != pNMUD);

        ProcessRatioSpinChange(pNMUD, pNMHDR->idFrom);
        break;
    default:
        bHandled = FALSE;
    }
    
    return 0;
}

LRESULT MainDlg::OnRatioBoxChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    return 0;
}
LRESULT MainDlg::OnRatioBoxSetFocus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    return 0;
}

LRESULT MainDlg::OnRatioBoxKillFocus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ATL::CString msg;
    ATL::CString buf;
    m_ctlRatioBox.GetWindowText(buf);
    int nRatio = std::wcstol(buf, NULL, 10);

	if (nRatio < eRatioMin)
	{
        msg.Format(_T("The compression ratio value '%d' is incorrect. Please, use value from range between 1 and 99."), nRatio);
		MessageBox(msg, _T("Error!"), MB_OK | MB_ICONEXCLAMATION);

	    buf.Format(_T("%02d"), eRatioMin);
		m_ctlRatioBox.SetWindowText(buf);
	}

	if (nRatio > eRatioMax)
	{
        msg.Format(_T("The compression ratio value '%d' is incorrect. Please, use value from range between 1 and 99."), nRatio);
		MessageBox(msg, _T("Error!"), MB_OK | MB_ICONEXCLAMATION);


	    buf.Format(_T("%02d"), eRatioMax);
		m_ctlRatioBox.SetWindowText(buf);
	}

    return 0;
}

LRESULT MainDlg::OnInputOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    // TODO - dodac rozszerzenie i filtr do zasobow

    m_pathInput.Empty();

    m_ctlFileProgress.SetPos(0);
    m_ctlProgressInfo.SetWindowText(_T("Ready!"));

    if (eModeSingle == m_mode)
    {
        // Single file selection

        CFileDialog dlg(TRUE, _T("sid"), NULL,
                        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                        _T("MrSID Files (*.sid)\0*.sid\0All Files (*.*)\0*.*\0"),
                        m_hWnd);
		if (IDOK == dlg.DoModal())
        {
            m_pathInput = dlg.m_szFileName;
            
            // Calculate output path
            int dotPos = m_pathInput.ReverseFind(_T('.'));
            m_pathOutput = m_pathInput.Left(dotPos);
            m_pathOutput.Append(_T(".jp2"));
            
            m_ctlPathInput.SetWindowText(m_pathInput);
            m_ctlPathOutput.SetWindowText(m_pathOutput);

            // Read and display image compression ratio
            std::string filePath = CT2A(m_pathInput);
            MrSidImageInfo sidInfo(filePath);
            MrSidImageInfo::int64 nInputRatio = sidInfo.GetCompressionRatio();

            ATL::CString msg;
            msg.Format(_T("%I64d"), nInputRatio);
            m_ctlInputRatioBox.SetWindowText(msg);

            UIEnable(IDC_START, true);
        }
    }
    else
    {
        // Directory selection

        CFolderDialog dlg(NULL, _T("Select directory with input files"), BIF_EDITBOX);
        if (IDOK == dlg.DoModal())
        {
            m_pathInput = dlg.GetFolderPath();
            m_ctlPathInput.SetWindowText(m_pathInput);
            m_ctlPathOutput.SetWindowText(m_pathInput);
        }
    }

    if (!m_pathInput.IsEmpty())
    {
        UIEnable(IDC_OPEN_OUTPUT, true);
        UIEnable(IDC_PATH_OUTPUT, true);
        UIEnable(IDC_START, true);
    }

    return 0;
}

LRESULT MainDlg::OnOutputOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    m_pathOutput.Empty();

    if (eModeSingle == m_mode)
    {
        // Single file selection
        CFileDialog dlg(FALSE, _T("jp2"), NULL,
                        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                        _T("JP2ECW Files (*.jp2)\0*.jp2\0All Files (*.*)\0*.*\0"),
                        m_hWnd);
		if (IDOK == dlg.DoModal())
        {
            m_pathOutput = dlg.m_szFileName;
            m_ctlPathOutput.SetWindowText(m_pathOutput);

            UIEnable(IDC_START, true);
        }
    }
    else
    {
        // Directory selection

        CFolderDialog dlg(NULL, _T("Select output directory"));
        if (IDOK == dlg.DoModal())
        {
            m_pathOutput = dlg.GetFolderPath();
            m_ctlPathOutput.SetWindowText(m_pathOutput);

            UIEnable(IDC_START, true);
        }
    }

    return 0;
}

LRESULT MainDlg::OnTranslationFinish(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    size_t counter = static_cast<size_t>(LOWORD(lParam));
    size_t from = static_cast<size_t>(HIWORD(lParam));

    m_worker.WaitUntilTerminate();
    if (m_worker.IsTerminated())
    {
        ATL::CString msg;
        if (counter > 0)
        {
            ATL::CString target(counter > 1 ? _T("files") : _T("file"));
            msg.Format(_T("Successfully translated %u of %u %s"),
                counter, from, target);
        }
        else
        {
          msg = _T("No MrSID files found in specified location!");
        }

        MessageBox(msg, _T("Finished!"), MB_OK | MB_ICONINFORMATION);

        UISetStateReady();
    }

    return 0;
}

LRESULT MainDlg::OnTranslationTargetRatio(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return 0;
}

LRESULT MainDlg::OnTranslationNext(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    size_t counter = static_cast<size_t>(wParam);
    dataset_t const* ds = reinterpret_cast<dataset_t const*>(lParam);

    ATL::CPath szInputFile(CA2T(ds->first.c_str()));
    ATL::CPath szOutputFile(CA2T(ds->second.c_str()));

    ATL::CString msg;
    msg.Format(_T("Source (%u/%u): %s"), (counter + 1), m_files.size(), szInputFile);
    m_ctlProgressInfo.SetWindowText(msg);

    msg.Format(_T("Target %s"), szOutputFile);
    m_ctlProgressFileInfo.SetWindowText(msg);

    return 0;
}

LRESULT MainDlg::OnTranslationProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    int percent = static_cast<int>(wParam);
    if (0 == percent)
    {
        m_ctlFileProgress.SetPos(0);
    }
    else
    {
        m_ctlFileProgress.StepIt();
    }

    return 0;
}

LRESULT MainDlg::OnTranslationFailure(UINT, WPARAM, LPARAM, BOOL&)
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Public Member Functions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Private Member Functions
/////////////////////////////////////////////////////////////////////////////

void MainDlg::UISetStateReady()
{
    ATLASSERT(::IsWindow(m_ctlPathInput));
    ATLASSERT(::IsWindow(m_ctlPathOutput));
    ATLASSERT(::IsWindow(m_ctlInputRatioBox));
    ATLASSERT(::IsWindow(m_ctlRatioBox));
    ATLASSERT(::IsWindow(m_ctlRatioSpin));
    ATLASSERT(::IsWindow(m_ctlFileProgress));
    ATLASSERT(::IsWindow(m_ctlProgressInfo));
    ATLASSERT(::IsWindow(m_ctlProgressFileInfo));

    if (::IsWindow(GetDlgItem(IDC_STOP)))
    {
        WTL::CButton btnStop(GetDlgItem(IDC_STOP));
        btnStop.SetDlgCtrlID(IDC_START);
        btnStop.SetWindowText(_T("Start"));
    }

    UIEnable(IDC_CLOSE, true);
    UIEnable(IDC_START, false);

    UISetCheck(IDC_MODE_BATCH_RECURSIVE, false);
    UISetRadio(IDC_MODE_BATCH, false, TRUE);
    UISetRadio(IDC_MODE_SINGLE, true, TRUE);

    UIEnable(IDC_MODE_SINGLE, true);
    UIEnable(IDC_MODE_BATCH, true);
    UIEnable(IDC_MODE_BATCH_RECURSIVE, false);

    // Initialize options group
    m_ctlInputRatioBox.SetWindowText(_T("0"));
    m_ctlRatioSpin.SetBuddy(m_ctlRatioBox);
    m_ctlRatioSpin.SetRange(eRatioMin, eRatioMax);
    m_ctlRatioSpin.SetPos(20);

    ATL::CString buf;
    buf.Format(_T("%02d"), m_ctlRatioSpin.GetPos());
    m_ctlRatioBox.SetWindowText(buf);

    UISetStateOptions(true);
    UISetCheck(IDC_OPT_RATIO_FROM_INPUT, false);
    UISetCheck(IDC_OPT_RATIO_FROM_INPUT, true);

    // Initialize input/output paths
    UISetStatePathBoxes(false);

    // Initialize progress controls
    UIResetProgressBar();

    // TODO: Move info messages to resources
    m_ctlProgressInfo.SetWindowText(_T("Ready!"));
    m_ctlProgressFileInfo.SetWindowText(_T(""));
}

void MainDlg::UISetStateBusy()
{
    if (::IsWindow(GetDlgItem(IDC_START)))
    {
        WTL::CButton btnStart(GetDlgItem(IDC_START));
        btnStart.SetDlgCtrlID(IDC_STOP);
        btnStart.SetWindowText(_T("Stop"));
    }

    UIEnable(IDC_CLOSE, false);
    UIEnable(IDC_STOP, true);

    UIEnable(IDC_MODE_SINGLE, false);
    UIEnable(IDC_MODE_BATCH, false);
    UIEnable(IDC_MODE_BATCH_RECURSIVE, false);

    UISetStatePathBoxes(true);
}

void MainDlg::UISetStatePathBoxes(BOOL bBusy)
{
    BOOL bEnable = bBusy ? false : true;

    UIEnable(IDC_OPEN_INPUT, bEnable);
    UIEnable(IDC_OPEN_OUTPUT, bEnable);
    UIEnable(IDC_PATH_INPUT, bEnable);
    UIEnable(IDC_PATH_OUTPUT, bEnable);

    if (!bBusy)
    {
        m_ctlPathInput.SetWindowText(_T(""));
        m_ctlPathOutput.SetWindowText(_T(""));
    }
}

void MainDlg::UISetStateOptions(BOOL bInputRatioActive)
{
    BOOL bInputEnable = bInputRatioActive;
    BOOL bTargetEnable = ! bInputRatioActive;

    // Input Compression Ratio
    UIEnable(IDC_OPT_INPUT_RATIO_BOX, bInputEnable);
    UIEnable(IDC_OPT_INPUT_RATIO_LABEL, bInputEnable);
    UIEnable(IDC_OPT_INPUT_RATIO_SUFFIX_LABEL, bInputEnable);

    // Target Compression Ratio
    UIEnable(IDC_OPT_TARGET_RATIO_LABEL, bTargetEnable);
    UIEnable(IDC_OPT_TARGET_RATIO_BOX, bTargetEnable);
    UIEnable(IDC_OPT_TARGET_RATIO_SPIN, bTargetEnable);
    UIEnable(IDC_OPT_TARGET_RATIO_SUFFIX_LABEL, bTargetEnable);
}

void MainDlg::UIResetProgressBar()
{
    // Initialize file processing progress controls
    m_ctlFileProgress.SetRange(0, 100);
    m_ctlFileProgress.SetPos(0);
    m_ctlFileProgress.SetStep(10);
}

void MainDlg::ProcessRatioSpinChange(LPNMUPDOWN pNMUD, UINT nID)
{
    ATLASSERT(NULL != pNMUD);
    ATLASSERT(::IsWindow(m_ctlRatioSpin));
    ATLASSERT(::IsWindow(m_ctlRatioBox));
    ATLASSERT(GetDlgItem(nID) == m_ctlRatioSpin);
    ATLASSERT(m_ctlRatioSpin.GetBuddy() == m_ctlRatioBox);

    ATL::CString buf;
    m_ctlRatioBox.GetWindowText(buf);

    int nRatio = std::wcstol(buf, NULL, 10);

    int nLower = 0;
    int nUpper = 0;
    m_ctlRatioSpin.GetRange(nLower, nUpper);

    if (pNMUD->iDelta > 0)
    {
        // Up button pressed
        ++nRatio;
        if (nRatio > nUpper)
            nRatio = nLower;
    }
    else if (pNMUD->iDelta < 0)
    {
        // Down button pressed
        --nRatio;
        if (nRatio < nLower)
            nRatio = nUpper;
    }

    buf.Format(_T("%02d"), nRatio);
    m_ctlRatioBox.SetWindowText(buf);
}

void MainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

void MainDlg::ClearDatasetList()
{
    std::vector<dataset_t> empty;
    m_files.swap(empty);

    m_ratios.clear();
}

BOOL MainDlg::IsTranslating() const
{
    // non-NULL values indicate the worker thread is active
    if (m_worker.IsRunning())
    {
        return TRUE;
    }

    return FALSE;
}

BOOL MainDlg::StopTranslation()
{
    BOOL bStopped = FALSE;

    if (IsTranslating())
    {
        ATL::CString msg(_T("Are you sure you want to abort the translation in progress?"));
        if (IDYES == MessageBox(msg, _T("Abort?"), MB_YESNO | MB_ICONQUESTION))
        {
            m_translator.Terminate();

            // TODO: Temporarily test with 30 sec timeout
            if (!m_worker.WaitUntilTerminate(30000))
            {
                // Termination failure, force to kill - unsafe!
                m_worker.Terminate();
            }

            if (m_worker.IsTerminated())
            {
                // Reset application state
                UISetStateReady();
                bStopped = TRUE;
            }
        }
    }

    return bStopped;
}

BOOL MainDlg::InitializeGDALDriver()
{
    ATL::CString msg;
    ATL::CString frmtOutput;
    frmtOutput.LoadString(IDS_GDAL_FRMT_OUTPUT);

    m_driver = GDALGetDriverByName(CT2A(frmtOutput));
    if (NULL == m_driver)
    {
        msg.Format(_T("Can not find output driver '%s'!"), frmtOutput);
        MessageBox(msg, _T("Error!"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Check driver capabilities 
    char** metaData  = GDALGetMetadata(m_driver, NULL);
    if (FALSE == CSLFetchBoolean(metaData, GDAL_DCAP_CREATECOPY, FALSE))
    {
        msg.Format(_T("Driver '%s' does not support CreateCopy operation!"), frmtOutput);
        MessageBox(msg, _T("Error!"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}
