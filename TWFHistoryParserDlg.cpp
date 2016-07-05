
// TWFHistoryParserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TWFHistoryParser.h"
#include "TWFHistoryParserDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTWFHistoryParserDlg dialog



CTWFHistoryParserDlg::CTWFHistoryParserDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TWFHISTORYPARSER_DIALOG, pParent)
	, m_szOutputFile(_T(""))
	, m_szCommTag(_T("TX"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTWFHistoryParserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szOutputFile);
	DDX_Text(pDX, IDC_EDIT2, m_szCommTag);
}

BEGIN_MESSAGE_MAP(CTWFHistoryParserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTWFHistoryParserDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTWFHistoryParserDlg message handlers

BOOL CTWFHistoryParserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTWFHistoryParserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTWFHistoryParserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTWFHistoryParserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

struct STR_DATA
{
	CString Volume;
	CString Open;
	CString High;
	CString Low;
	CString Close;
};

void SaveData(CStdioFile &file, STR_DATA &data, CString szDate)
{
	CString szLine = szDate;
	szLine += _T(",");

	szLine += _T("08:46:00");
	szLine += _T(",");

	szLine += data.Open;
	szLine += _T(",");

	szLine += data.High;
	szLine += _T(",");

	szLine += data.Low;
	szLine += _T(",");

	szLine += data.Close;
	szLine += _T(",");

	szLine += data.Volume;
	szLine += _T("\r\n");

	file.WriteString(szLine);
}

void FillDataTitle(CStdioFile &file)
{
	file.WriteString(_T("Date,Time,Open,High,Low,Close,TotalVolume\r\n"));
}

void CTWFHistoryParserDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);

	CFileDialog filedlg(TRUE);
	if (filedlg.DoModal() == IDOK)
	{
		CString szInputFile = filedlg.GetPathName();
		CString szPath = filedlg.GetFolderPath();

		CStdioFile InFile(szInputFile, CFile::modeRead);
		CStdioFile OutFile(szPath+_T("\\")+m_szOutputFile, CFile::modeWrite| CFile::modeCreate);
		FillDataTitle(OutFile);
		
		CString szCurDate;
		STR_DATA data;
		bool bDataFilled = false;
		//std

		CString szLine;
		int nLine = 0;
		while (InFile.ReadString(szLine))
		{
			if (szLine.IsEmpty())
				continue;

			if (nLine++) // ignore first line
			{
				int nToken = 0;
				CString szDate = szLine.Tokenize(_T(","), nToken);
				CString szTag = szLine.Tokenize(_T(","), nToken);
				CString szIgnore;
				szIgnore = szLine.Tokenize(_T(","), nToken); // 到期年月
				if (szTag == m_szCommTag && szIgnore.GetLength() == 6)
				{
					if (szDate != szCurDate)
					{
						if (bDataFilled)
						{
							SaveData(OutFile, data, szCurDate);
							bDataFilled = false;
						}

						data.Open = szLine.Tokenize(_T(","), nToken);
						data.High = szLine.Tokenize(_T(","), nToken);
						data.Low = szLine.Tokenize(_T(","), nToken);
						data.Close = szLine.Tokenize(_T(","), nToken);
						szIgnore = szLine.Tokenize(_T(","), nToken); // 漲跌價
						szIgnore = szLine.Tokenize(_T(","), nToken); // 漲跌%
						data.Volume = szLine.Tokenize(_T(","), nToken);
						bDataFilled = true;
					}


					szCurDate = szDate;
				}
			}
		}

		if (bDataFilled)
		{
			SaveData(OutFile, data, szCurDate);
			bDataFilled = false;
		}


	}
}
