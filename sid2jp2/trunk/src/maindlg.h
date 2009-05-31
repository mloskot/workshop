// $Id$
/////////////////////////////////////////////////////////////////////////////
// MainDlg.h : interface of the MainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef SID2JP2_MAINDLG_H_INCLUDED
#define SID2JP2_MAINDLG_H_INCLUDED

#include "dataset.h"
#include "thread.h"
#include "translator.h"
#include "mrsidimageinfo.h"
// gdal
#include <gdal.h>
// std
#include <cwchar>
#include <map>
#include <string>
#include <vector>
#include <utility>

//
// Main dialog box class.
//
class MainDlg :
    public CDialogImpl<MainDlg>,
    public CUpdateUI<MainDlg>,
    public CMessageFilter,
    public CIdleHandler,
    public CWinDataExchange<MainDlg>
{
public:
	
    enum { IDD = IDD_MAINDLG };

    /// Default constructor.
    /// Constructor does only initialize some of data variables.
    /// OnInitDialog is responsible for UI initialization.
    MainDlg();

    //
    // Dynamic Data Exchange map
    //
    BEGIN_DDX_MAP(MainDlg)
        DDX_CONTROL_HANDLE(IDC_PATH_INPUT, m_ctlPathInput)
        DDX_CONTROL_HANDLE(IDC_PATH_OUTPUT, m_ctlPathOutput)
        DDX_CONTROL_HANDLE(IDC_OPT_INPUT_RATIO_BOX, m_ctlInputRatioBox)
        DDX_CONTROL_HANDLE(IDC_OPT_TARGET_RATIO_SPIN, m_ctlRatioSpin)
        DDX_CONTROL_HANDLE(IDC_OPT_TARGET_RATIO_BOX, m_ctlRatioBox)
        DDX_CONTROL_HANDLE(IDC_PROGRESS_PER_FILE, m_ctlFileProgress)
        DDX_CONTROL_HANDLE(IDC_PROGRESS_INFO, m_ctlProgressInfo)
        DDX_CONTROL_HANDLE(IDC_PROGRESS_FILE_INFO, m_ctlProgressFileInfo)
        DDX_TEXT(IDC_PATH_INPUT, m_pathInput)
        DDX_TEXT(IDC_PATH_OUTPUT, m_pathOutput)
    END_DDX_MAP()

    //
    // UI Updating Controls map
    //
	BEGIN_UPDATE_UI_MAP(MainDlg)
        UPDATE_ELEMENT(IDC_START, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_STOP, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_CLOSE, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_MODE_SINGLE, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_MODE_BATCH, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_MODE_BATCH_RECURSIVE, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPEN_INPUT, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPEN_OUTPUT, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_PATH_INPUT, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_PATH_OUTPUT, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPT_RATIO_FROM_INPUT, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPT_INPUT_RATIO_BOX, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPT_INPUT_RATIO_LABEL, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPT_INPUT_RATIO_SUFFIX_LABEL, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPT_TARGET_RATIO_BOX, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPT_TARGET_RATIO_SPIN, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPT_TARGET_RATIO_LABEL, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPT_TARGET_RATIO_SUFFIX_LABEL, UPDUI_CHILDWINDOW)
	END_UPDATE_UI_MAP()

    //
    // Windows Messages map
    //
	BEGIN_MSG_MAP_EX(MainDlg)

        // Custom SID2JP2 Messages
        MESSAGE_HANDLER(sid2jp2::WM_SID2JP2_FILE_PROGRESS, OnTranslationProgress)
        MESSAGE_HANDLER(sid2jp2::WM_SID2JP2_FILE_NEXT, OnTranslationNext)
        MESSAGE_HANDLER(sid2jp2::WM_SID2JP2_TARGET_RATIO, OnTranslationTargetRatio)
        MESSAGE_HANDLER(sid2jp2::WM_SID2JP2_FILE_FAILURE, OnTranslationFailure)
        MESSAGE_HANDLER(sid2jp2::WM_SID2JP2_FINISH, OnTranslationFinish)
        

        // Windows Messages
		MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SYSCOMMAND(OnSysCommand)
        MESSAGE_HANDLER(WM_NOTIFY, OnRatioSpinChanged)

        // User-Interface Commands
        COMMAND_HANDLER(IDC_OPT_TARGET_RATIO_BOX, EN_CHANGE, OnRatioBoxChanged)
        COMMAND_HANDLER(IDC_OPT_TARGET_RATIO_BOX, EN_SETFOCUS, OnRatioBoxSetFocus)
		COMMAND_HANDLER(IDC_OPT_TARGET_RATIO_BOX, EN_KILLFOCUS, OnRatioBoxKillFocus)
        COMMAND_HANDLER(IDC_OPT_RATIO_FROM_INPUT, BN_CLICKED, OnRatioFromInputChanged)
        COMMAND_HANDLER(IDC_MODE_BATCH_RECURSIVE, BN_CLICKED, OnModeRecursiveChanged)
        COMMAND_RANGE_HANDLER(IDC_MODE_SINGLE, IDC_MODE_BATCH, OnModeChanged)
        COMMAND_ID_HANDLER(IDC_OPEN_INPUT, OnInputOpen)
        COMMAND_ID_HANDLER(IDC_OPEN_OUTPUT, OnOutputOpen)
        COMMAND_ID_HANDLER(IDC_START, OnStart)
        COMMAND_ID_HANDLER(IDC_STOP, OnStop)
        COMMAND_ID_HANDLER(IDC_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
        COMMAND_ID_HANDLER(ID_APP_HELP, OnAppHelp)

        CHAIN_MSG_MAP(CUpdateUI<MainDlg>)
    END_MSG_MAP()

    virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

    //
    // SID2JP2 Translation Message handlers
    //
    LRESULT OnTranslationFinish(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnTranslationTargetRatio(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnTranslationFailure(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnTranslationNext(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnTranslationProgress(UINT, WPARAM, LPARAM, BOOL&);

    //
    // Windows Message and Commands handlers
    //
	LRESULT OnInitDialog(HWND, LPARAM);
    void OnSysCommand(UINT, WTL::CPoint);

    LRESULT OnStart(WORD, WORD wID, HWND, BOOL&);
    LRESULT OnStop(WORD, WORD wID, HWND, BOOL&);
	
    LRESULT OnClose(WORD, WORD, HWND, BOOL&);
    LRESULT OnAppAbout(WORD, WORD, HWND, BOOL&);
    LRESULT OnAppHelp(WORD, WORD, HWND, BOOL&);
    
    LRESULT OnInputOpen(WORD, WORD, HWND, BOOL&);
    LRESULT OnOutputOpen(WORD, WORD, HWND, BOOL&);

    LRESULT OnRatioFromInputChanged(WORD, WORD, HWND, BOOL&);
    LRESULT OnRatioSpinChanged(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnRatioBoxChanged(WORD, WORD, HWND, BOOL&);
    LRESULT OnRatioBoxSetFocus(WORD, WORD, HWND, BOOL&);
    LRESULT OnRatioBoxKillFocus(WORD, WORD, HWND, BOOL&);
    
    LRESULT OnModeChanged(WORD, WORD, HWND, BOOL&);
    LRESULT OnModeRecursiveChanged(WORD, WORD, HWND, BOOL&);

    //
    // Other Public Interface
    //

private:

    //
    // Internal Data
    //

    enum ProcessingMode
    {
        eModeNone   = -1,
        eModeSingle = 0,
        eModeBatch  = 1,
        eModeBatchRecursive = 2
    };
    ProcessingMode m_mode;

    enum CompressionRatioMode
    {
        eInputFileRatio,
        eUserRatio
    };
    CompressionRatioMode m_ratioMode;

	enum CompressionRatioRange
	{
		eRatioMin = 1,
		eRatioMax = 99,
	};

    enum CacheValue
    {
        eCacheMax = 100000000
    };

    ATL::CString m_pathInput;
    ATL::CString m_pathOutput;
    GDALDriverH m_driver;
    std::vector<sid2jp2::dataset_t> m_files;
    std::map<std::string, sid2jp2::MrSidImageInfo::int64> m_ratios;
    
    sid2jp2::Translator m_translator;
    sid2jp2::Thread<sid2jp2::Translator> m_worker;

    //
    // User Interface Controls
    //

    WTL::CEdit m_ctlPathInput;
    WTL::CEdit m_ctlPathOutput;
    WTL::CEdit m_ctlInputRatioBox;
    WTL::CEdit m_ctlRatioBox;
    WTL::CUpDownCtrl m_ctlRatioSpin;
    WTL::CStatic m_ctlProgressInfo;
    WTL::CStatic m_ctlProgressFileInfo;
    WTL::CProgressBarCtrl m_ctlFileProgress;

    //
    // Private Functions
    //

    void UISetStateReady();
    void UISetStateBusy();
    void UISetStatePathBoxes(BOOL bBusy);
    void UISetStateOptions(BOOL bInputRatioActive);
    void UIResetProgressBar();
    void CloseDialog(int nVal);
    void ProcessRatioSpinChange(LPNMUPDOWN pNMUD, UINT nID);
    void ClearDatasetList();

    BOOL IsTranslating() const;
    BOOL StopTranslation();
    BOOL InitializeGDALDriver();
    BOOL ProcessFile(const char* inputFile, const char* outputFile, char** options);

};

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

#endif // SID2JP2_MAINDLG_H_INCLUDED