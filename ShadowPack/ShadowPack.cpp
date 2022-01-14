
// ShadowPack.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "ShadowPack.h"
#include "ShadowPackDlg.h"
#include "PackUtils.h"
#include "MediaFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShadowPackApp

BEGIN_MESSAGE_MAP(CShadowPackApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CShadowPackApp 构造

CShadowPackApp::CShadowPackApp():
	m_bRestart(FALSE)
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CShadowPackApp 对象

CShadowPackApp theApp;


// CShadowPackApp 初始化

BOOL CShadowPackApp::InitInstance()
{

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	CShadowPackDlg dlg;
	INT_PTR nResponse;
	CConfigManager::CONFIG_VALUE_T val;
	CString strIniFilePath;

	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	
	if (!InitCommonControlsEx(&InitCtrls)) {
		goto err;
	}

	if (!CWinApp::InitInstance()) {
		goto err;
	}

	if (!m_Locale.Initialize()) {
		goto err;
	}

	CMediaFactory::LoadMediaInfo();

	//m_Config.SetConfigReg(HKEY_CURRENT_USER, _T("Software\\ShadowPack"));

	strIniFilePath = CPackUtils::GetWorkingPath();
	strIniFilePath += _T("ShadowPack.ini");
	TRACE(_T("Use INI file %s\n"), strIniFilePath);
	m_Config.SetConfigFile(strIniFilePath);
	m_Config.CreateDefault();
	m_Config.DumpConfig();


	if (m_Config.GetConfig(_T("main"), _T("locale"), val)) {
		m_Locale.SetLocale((CLocaleManager::LOCAL_ID_T)val.n8);
	} else {
		goto err;
	}

	AfxEnableControlContainer();

	m_pMainWnd = &dlg;
	nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
	}


#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	if (m_bRestart) {
		RestartApp();
	}

	CMediaFactory::DestoryMediaInfo();
err:
	return FALSE;
}

void CShadowPackApp::RestartApp()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR FilePath[255];


	::ZeroMemory(&si, sizeof(si));
	::ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	if (::GetModuleFileName(NULL, FilePath, _countof(FilePath))) {
		FilePath[_countof(FilePath) - 1] = 0;

		if (::CreateProcess(NULL, FilePath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}
	}
}

