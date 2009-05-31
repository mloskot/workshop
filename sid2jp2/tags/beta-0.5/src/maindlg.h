// $Id$
/////////////////////////////////////////////////////////////////////////////
// MainDlg.h : interface of the MainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef SID2JP2_MAINDLG_H_INCLUDED
#define SID2JP2_MAINDLG_H_INCLUDED

// gdal
#include <gdal.h>
// std
#include <cwchar>
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

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

    //
    // Dynamic Data Exchange map
    //
    BEGIN_DDX_MAP(MainDlg)
        DDX_CONTROL_HANDLE(IDC_PATH_INPUT, m_ctlPathInput)
        DDX_CONTROL_HANDLE(IDC_PATH_OUTPUT, m_ctlPathOutput)
        DDX_CONTROL_HANDLE(IDC_OPT_RATIO_SPIN, m_ctlRatioSpin)
        DDX_CONTROL_HANDLE(IDC_OPT_RATIO_BOX, m_ctlRatioBox)
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
        UPDATE_ELEMENT(IDC_MODE_SINGLE, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_MODE_BATCH, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_MODE_BATCH_RECURSIVE, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPEN_INPUT, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_OPEN_OUTPUT, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_PATH_INPUT, UPDUI_CHILDWINDOW)
        UPDATE_ELEMENT(IDC_PATH_OUTPUT, UPDUI_CHILDWINDOW)
	END_UPDATE_UI_MAP()

    //
    // Windows Messages map
    //
	BEGIN_MSG_MAP_EX(MainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
        //NOTIFY_HANDLER(IDC_OPT_RATIO_SPIN, UDN_DELTAPOS, OnRatioSpinChanged2)
        MESSAGE_HANDLER(WM_NOTIFY, OnRatioSpinChanged)
        COMMAND_HANDLER(IDC_OPT_RATIO_BOX, EN_CHANGE, OnRatioBoxChanged)
        COMMAND_HANDLER(IDC_OPT_RATIO_BOX, EN_SETFOCUS, OnRatioBoxFocus)
        COMMAND_RANGE_HANDLER(IDC_MODE_SINGLE, IDC_MODE_BATCH, OnModeChanged)
        COMMAND_HANDLER(IDC_MODE_BATCH_RECURSIVE, BN_CLICKED, OnModeRecursiveChanged)
        COMMAND_ID_HANDLER(IDC_OPEN_INPUT, OnInputOpen)
        COMMAND_ID_HANDLER(IDC_OPEN_OUTPUT, OnOutputOpen)
        COMMAND_ID_HANDLER(IDC_RUN, OnRun)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
        COMMAND_ID_HANDLER(ID_APP_HELP, OnAppHelp)
        CHAIN_MSG_MAP(CUpdateUI<MainDlg>)
    END_MSG_MAP()

    //
    // Windows Message and Commands handlers
    //
	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);
    LRESULT OnRun(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnAppAbout(WORD, WORD, HWND, BOOL& );
    LRESULT OnAppHelp(WORD, WORD, HWND, BOOL& );
    LRESULT OnModeChanged(WORD, WORD, HWND, BOOL& );
    LRESULT OnModeRecursiveChanged(WORD, WORD, HWND, BOOL& );
    LRESULT OnRatioSpinChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
    LRESULT OnRatioBoxChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnRatioBoxFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnInputOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnOutputOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    //
    // Other Public Interface
    //
    static int __stdcall FileProcessingCallback(double dfComplete,
        const char* pszMessage, void* pProgressArg);

private:

    //
    // User Interface Controls
    //
    WTL::CEdit m_ctlPathInput;
    WTL::CEdit m_ctlPathOutput;
    WTL::CEdit m_ctlRatioBox;
    WTL::CUpDownCtrl m_ctlRatioSpin;
    WTL::CStatic m_ctlProgressInfo;
    WTL::CStatic m_ctlProgressFileInfo;
    WTL::CProgressBarCtrl m_ctlFileProgress;

    //
    // Internal Data
    //
    enum ProcessingMode
    {
        eModeSingle = 0,
        eModeBatch = 1,
        eModeBatchRecursive = 2
    };
    ProcessingMode m_mode;

    enum CacheValue
    {
        eCacheMax = 100000000
    };

    ATL::CString m_pathInput;
    ATL::CString m_pathOutput;

    GDALDriverH m_hDriver;

    typedef std::pair<std::string, std::string> dataset_t;
    std::vector<dataset_t> m_files;

    //
    // Private Function Members
    //
    void UIResetState();
    void UIResetProgressBar();
    void UIResetPathBoxes();
    void CloseDialog(int nVal);
    void ProcessRatioSpinChange(LPNMUPDOWN pNMUD, UINT nID);


    void ClearDatasetList();
    BOOL InitializeGDALDriver();
    BOOL ProcessFile(const char* inputFile, const char* outputFile, char** options);

};

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

#endif // SID2JP2_MAINDLG_H_INCLUDED