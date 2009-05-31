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
// gdal
#include <gdal.h>
#include <cpl_error.h>
#include <cpl_string.h>
// std
#include <cmath>
#include <string>
#include <vector>
#include <utility>
// boost
#include <boost/lexical_cast.hpp>
// winstl
#include <winstl/findfile_sequence.hpp>

using namespace sid2jp2;

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

LRESULT MainDlg::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
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
    UIResetState();

    // Initialize data
    m_mode = eModeSingle;
    m_hDriver = NULL;

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

LRESULT MainDlg::OnRun(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    BOOL ret = DoDataExchange(DDX_SAVE);
    ATLASSERT(TRUE == ret);

    // Common message buffor
    ATL::CString msg;

    //WTL::CWaitCursor wait(TRUE);
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
    std::string targetRatio;
    try
    {
        ATL::CString buf;
        m_ctlRatioBox.GetWindowText(buf);
        int ratio = std::wcstol(buf, NULL, 10);

        // Calculate compression percentage
        int perc = (100 - (100 / ratio));
        targetRatio = boost::lexical_cast<std::string>(perc);
    }
    catch(boost::bad_lexical_cast& e)
    {
        std::string errMsg(e.what());
        MessageBox(_T("Invalid value of Target Compression Ratio!"),
                   _T("Error!"), MB_OK | MB_ICONERROR);
        return 0;
    }

    char** copyOptions = NULL;
    copyOptions = ::CSLSetNameValue(copyOptions, "TARGET", targetRatio.c_str());
    copyOptions = ::CSLSetNameValue(copyOptions, "LARGE_OK", "YES");

    if (eModeSingle == m_mode && m_files.size() > 1)
    {
        msg.Format(_T("There are too many files selected for single mode processing!!"));
        MessageBox(msg, _T("Error!"), MB_OK | MB_ICONERROR);
        return 0;
    }

    //
    // Generate list of input MrSID files
    //

    // TODO: mloskot - replace hardcoded strings with resources

    m_ctlProgressInfo.SetWindowText(_T("Generating input files list..."));

    // Prepare dataset collection
    ClearDatasetList();

    if (m_pathInput.IsEmpty() || m_pathOutput.IsEmpty())
    {
        m_ctlProgressInfo.SetWindowText(_T("Ready!"));
        msg.Format(_T("Please, select source and target dataset!"));
        MessageBox(msg, _T("No dataset!"), MB_OK | MB_ICONSTOP);
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
        fs::generate_file_list_recurse(inputPath, "*.sid", tmpList);

        // Rebuild basic list to colletion of input/output paths
        fs::generate_dataset_list(inputPath, tmpList, outputPath, m_files, ".jp2", true);

    }
    else
    {
        // NEVER SHOULD GET HERE
        assert("[sid2jp2] Unknown processing mode");
    }

    //
    // Process collection of files
    //
    std::vector<dataset_t>::size_type filesCounter = 0;

    for (std::vector<dataset_t>::const_iterator it = m_files.begin();
         it != m_files.end(); ++it)
    {
        std::string const& fileInput = it->first;
        std::string const& fileOutput = it->second;

        // Update progress info
        ATL::CPath szInputFile(CA2T(fileInput.c_str()));
        ATL::CPath szOutputFile(CA2T(fileOutput.c_str()));

        msg.Format(_T("Source (%u/%u): %s"),
                   (filesCounter + 1), m_files.size(), szInputFile);
        m_ctlProgressInfo.SetWindowText(msg);
        msg.Format(_T("Target %s"), szOutputFile);
        m_ctlProgressFileInfo.SetWindowText(msg);

        // Check if the output directory exists, otherwise
        // create it recursively, from output file path.
        fs::create_file_path_recurse(fileOutput);

        // Translate MrSID file to JPEG200 ECW
        ret = ProcessFile(fileInput.c_str(), fileOutput.c_str(), copyOptions);
        if (!ret)
        {
            error::file_translation_failure(m_hWnd, fileInput);

            // TODO: mloskot - Make exit on failure optional
            return 0;
        }

        ++filesCounter;
    }

    //
    // Update processing summary
    //

    // TODO - Replace with msg box
    ATL::CString target(filesCounter > 1 ? _T("files") : _T("file"));
    msg.Format(_T("Successfully translated %u of %u %s"),
               filesCounter, m_files.size(), target);
    
    MessageBox(msg, _T("Finished!"), MB_OK | MB_ICONINFORMATION);

    //
    // Reset application state
    //
    UIResetState();
    ClearDatasetList();

    return 0;
}

LRESULT MainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CloseDialog(wID);
	return 0;
}

LRESULT MainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	AboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT MainDlg::OnAppHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    MessageBox(_T("Simple manual will be here available"), _T("Help!"), MB_OK);
	return 0;
}

LRESULT MainDlg::OnModeChanged(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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
        UIResetPathBoxes();
    }

    return 0;
}

LRESULT MainDlg::OnModeRecursiveChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (IsDlgButtonChecked(IDC_MODE_BATCH_RECURSIVE))
        m_mode = eModeBatchRecursive;
    else
        m_mode = eModeBatch;

    return 0;
}

LRESULT MainDlg::OnRatioSpinChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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

LRESULT MainDlg::OnRatioBoxChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    return 0;
}
LRESULT MainDlg::OnRatioBoxFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    return 0;
}

LRESULT MainDlg::OnInputOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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
        }
    }
    else
    {
        // Directory selection

        CFolderDialog dlg(NULL, _T("Select directory with input files"));
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
    }

    return 0;
}

LRESULT MainDlg::OnOutputOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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
        }
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Public Member Functions
/////////////////////////////////////////////////////////////////////////////

int CPL_STDCALL MainDlg::FileProcessingCallback(double dfComplete,
                                                const char* pszMessage,
                                                void* pProgressArg)
{
    ATLASSERT(NULL != pProgressArg);

    WTL::CProgressBarCtrl* pCtlProgress = NULL;
    if (NULL != pProgressArg)
    {
        pCtlProgress = static_cast<WTL::CProgressBarCtrl*>(pProgressArg);
        ATLASSERT(::IsWindow(*pCtlProgress));
    }

    static double dfLastComplete = -1.0;

    if (dfLastComplete > dfComplete)
    {
        if (dfLastComplete >= 1.0)
            dfLastComplete = -1.0;
        else
            dfLastComplete = dfComplete;
    }

    if (std::floor(dfLastComplete * 10) != std::floor(dfComplete * 10))
    {
        int nPercent = static_cast<int>(std::floor(dfComplete * 100));
        
        // TODO: Testing only
        ATL::CString m;
        m.Format(_T("Percent: %d\n"), nPercent);
        ::OutputDebugString(m);

        if (0 == nPercent)
        {
            pCtlProgress->SetPos(0);
        }
        else
        {
            // if (nPercent > 0 && nPercent < 100)
            pCtlProgress->StepIt();
        }
    }

    dfLastComplete = dfComplete;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Private Member Functions
/////////////////////////////////////////////////////////////////////////////

void MainDlg::UIResetState()
{
    ATLASSERT(::IsWindow(m_ctlPathInput));
    ATLASSERT(::IsWindow(m_ctlPathOutput));
    ATLASSERT(::IsWindow(m_ctlRatioBox));
    ATLASSERT(::IsWindow(m_ctlRatioSpin));
    ATLASSERT(::IsWindow(m_ctlFileProgress));
    ATLASSERT(::IsWindow(m_ctlProgressInfo));
    ATLASSERT(::IsWindow(m_ctlProgressFileInfo));

    UISetCheck(IDC_MODE_BATCH_RECURSIVE, false);
    UISetRadio(IDC_MODE_BATCH, false, TRUE);
    UISetRadio(IDC_MODE_SINGLE, true, TRUE);

    UIEnable(IDC_MODE_SINGLE, true);
    UIEnable(IDC_MODE_BATCH, true);
    UIEnable(IDC_MODE_BATCH_RECURSIVE, false);

    // Initialize target ratio spin button
    m_ctlRatioSpin.SetBuddy(m_ctlRatioBox);
    m_ctlRatioSpin.SetRange(0, 100);
    m_ctlRatioSpin.SetPos(20);

    ATL::CString buf;
    buf.Format(_T("%02d"), m_ctlRatioSpin.GetPos());
    m_ctlRatioBox.SetWindowText(buf);

    // Initialize input/output paths
    UIResetPathBoxes();

    // Initialize progress controls
    UIResetProgressBar();

    // TODO: Move info messages to resources
    m_ctlProgressInfo.SetWindowText(_T("Ready!"));
    m_ctlProgressFileInfo.SetWindowText(_T(""));
}

void MainDlg::UIResetProgressBar()
{
    // Initialize file processing progress controls
    m_ctlFileProgress.SetRange(0, 100);
    m_ctlFileProgress.SetPos(0);
    m_ctlFileProgress.SetStep(10);
}

void MainDlg::UIResetPathBoxes()
{
    UIEnable(IDC_OPEN_INPUT, true);
    UIEnable(IDC_PATH_INPUT, true);
    UIEnable(IDC_OPEN_OUTPUT, false);
    UIEnable(IDC_PATH_OUTPUT, false);

    m_ctlPathInput.SetWindowText(_T(""));
    m_ctlPathOutput.SetWindowText(_T(""));
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
        if (nRatio <= nLower)
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
}

BOOL MainDlg::InitializeGDALDriver()
{
    ATL::CString msg;
    ATL::CString frmtOutput;
    frmtOutput.LoadString(IDS_GDAL_FRMT_OUTPUT);

    m_hDriver = GDALGetDriverByName(CT2A(frmtOutput));
    if (NULL == m_hDriver)
    {
        msg.Format(_T("Can not find output driver '%s'!"), frmtOutput);
        MessageBox(msg, _T("Error!"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Check driver capabilities 
    char** metaData  = GDALGetMetadata(m_hDriver, NULL);
    if (FALSE == CSLFetchBoolean(metaData, GDAL_DCAP_CREATECOPY, FALSE))
    {
        msg.Format(_T("Driver '%s' does not support CreateCopy operation!"), frmtOutput);
        MessageBox(msg, _T("Error!"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}

BOOL MainDlg::ProcessFile(const char* inputFile, const char* outputFile,
                          char** options)
{
    GDALDatasetH hSrcDS = NULL;
    GDALDatasetH hDstDS = NULL;

    ATLASSERT(NULL != m_hDriver);
    ATLASSERT(NULL != inputFile && NULL != outputFile);

    hSrcDS = GDALOpen(inputFile, GA_ReadOnly);
    if (NULL != hSrcDS)
    {
        hDstDS = GDALCreateCopy(m_hDriver, outputFile, hSrcDS, FALSE, options,
                                &MainDlg::FileProcessingCallback,
                                &m_ctlFileProgress);

        if (NULL != hDstDS)
        {
            GDALClose(hDstDS);

            // SUCCESS
            return TRUE;
        }
    }

    // FAILURE
    return FALSE;
}