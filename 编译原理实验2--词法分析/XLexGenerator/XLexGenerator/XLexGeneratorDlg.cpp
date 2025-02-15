
// XLexGeneratorDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "XLexGenerator.h"
#include "XLexGeneratorDlg.h"
#include "afxdialogex.h"
#include "ShowLEX.h"
#include "TransitionTable.h"
#include "MyStack.h"
#include "Graph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LStack LinkedStack

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

Graph *NFATable;
TransitionTable *DFATable;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CXLexGeneratorDlg 对话框



CXLexGeneratorDlg::CXLexGeneratorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_XLEXGENERATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXLexGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REGEXP, Regexp);
	DDX_Control(pDX, IDC_SHOW, ShowDate);
}

BEGIN_MESSAGE_MAP(CXLexGeneratorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NFA, &CXLexGeneratorDlg::OnBnClickedNfa)
	ON_BN_CLICKED(IDC_DFA, &CXLexGeneratorDlg::OnBnClickedDfa)
	ON_BN_CLICKED(IDC_MINDFA, &CXLexGeneratorDlg::OnBnClickedMinDfa)
	ON_BN_CLICKED(IDC_PROGRAME, &CXLexGeneratorDlg::OnBnClickedPrograme)
	ON_BN_CLICKED(IDC_OPEN, &CXLexGeneratorDlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_SAVE, &CXLexGeneratorDlg::OnBnClickedSave)
END_MESSAGE_MAP()


// CXLexGeneratorDlg 消息处理程序

BOOL CXLexGeneratorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	ShowDate.SetExtendedStyle(ShowDate.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	Regexp.GetWindowTextW(preRegexp);
	init();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CXLexGeneratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CXLexGeneratorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CXLexGeneratorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CXLexGeneratorDlg::OnBnClickedNfa()
{
	// TODO: 在此添加控件通知处理程序代码
	Regexp.GetWindowTextW(nowRegexp);
	if (nowRegexp != preRegexp || !hasNFA)    //之前没生成过，并且当前正则和之前不一样，则生成
	{
		init();
		preRegexp = nowRegexp;
		hasNFA = true;
		GetRegExp();
		InsertCatNode();
		RegExpToPost();
		GetEdgeNumber();
		ThompsonConstruction();
	}
	NFATable->OutputNFA(&ShowDate);
}


void CXLexGeneratorDlg::OnBnClickedDfa()
{
	// TODO: 在此添加控件通知处理程序代码
	Regexp.GetWindowTextW(nowRegexp);
	if (nowRegexp != preRegexp || !hasDFA)    //之前没生成过，并且当前正则和之前不一样，则生成
	{
		init();
		// 析构之前的DStates和Dtran表以及AcceptStates数组
		if (DStates != NULL || Dtran != NULL || AcceptStates != NULL)
		{
			for (int i = 0; i < NFAStatesNumber + 1; i++)
			{
				if(DStates != NULL) delete[] DStates[i];
				if (Dtran != NULL) delete[] Dtran[i];
			}
			if (DStates != NULL) delete[] DStates;
			if (Dtran != NULL) delete[] Dtran;
			if (AcceptStates != NULL) delete[] AcceptStates;
		}
		preRegexp = nowRegexp;
		hasDFA = true;
		GetRegExp();
		InsertCatNode();
		RegExpToPost();
		GetEdgeNumber();
		ThompsonConstruction();
		SubsetConstruction();
	}
	//闭包子集
	int i, j;
	CString *states = new CString[DStatesNumber];
	CString s1, s2;
	for (i = 0; i < DStatesNumber; i++)
	{
		s1 = L"";
		j = 0;
		while (DStates[i][j] != -1)
		{
			s2.Format(_T("%d,"), DStates[i][j]);
			s1 += s2;
			j++;
		}
		states[i] = s1;
	}
	// 输出DFA状态转换表
	while (ShowDate.DeleteColumn(0));
	ShowDate.DeleteAllItems();
	ShowDate.InsertColumn(0, L"状态\\字符", LVCFMT_CENTER, 120);
	for (j = 0; j < edgeNumber; j++)
	{
		CString s(edge[j]);
		ShowDate.InsertColumn(j + 1, s, LVCFMT_CENTER, 120);
	}

	for (i = 0; i < DtranNumber; i++)
	{
		ShowDate.InsertItem(i, states[i]);
		for (j = 0; j < edgeNumber; j++)
		{
			if (Dtran[i][j] < 0)
			{
				ShowDate.SetItemText(i, 1 + j, L"");
			}
			else
			{
				ShowDate.SetItemText(i, 1 + j, states[Dtran[i][j]]);
			}
		}
	}
}


void CXLexGeneratorDlg::OnBnClickedMinDfa()
{
	// TODO: 在此添加控件通知处理程序代码
	Regexp.GetWindowTextW(nowRegexp);
	if (nowRegexp != preRegexp || !hasMinDFA)    //之前没生成过，并且当前正则和之前不一样，则生成
	{
		init();
		// 析构之前的DStates和Dtran表以及AcceptStates数组
		if (DStates != NULL || Dtran != NULL || AcceptStates != NULL)
		{
			for (int i = 0; i < NFAStatesNumber + 1; i++)
			{
				if (DStates != NULL) delete[] DStates[i];
				if (Dtran != NULL) delete[] Dtran[i];
			}
			if (DStates != NULL) delete[] DStates;
			if (Dtran != NULL) delete[] Dtran;
			if (AcceptStates != NULL) delete[] AcceptStates;
		}
		
		preRegexp = nowRegexp;
		hasMinDFA = true;
		GetRegExp();
		InsertCatNode();
		RegExpToPost();
		GetEdgeNumber();
		ThompsonConstruction();
		SubsetConstruction();
	}
	int i, j;
	// 反复检测Dtran表中是否能化简，能则继续化简
	int DtranNumberAfterMinimization = MinimizeDFAStates(Dtran, AcceptStates, DtranNumber, edgeNumber);
	while (DtranNumberAfterMinimization != DtranNumber)
	{
		DtranNumber = DtranNumberAfterMinimization;
		DtranNumberAfterMinimization = MinimizeDFAStates(Dtran, AcceptStates, DtranNumber, edgeNumber);
	}
	// 将Dtran表的内容拷贝到DFA表
	DFATable = new TransitionTable(DtranNumber, edgeNumber);
	for (i = 0; i < DtranNumber; i++)
	{
		for (j = 0; j < edgeNumber; j++)
		{
			DFATable->SetValue(i, j, Dtran[i][j]);
		}
	}
	// 输出最小DFA状态表
	while (ShowDate.DeleteColumn(0));
	ShowDate.DeleteAllItems();
	ShowDate.InsertColumn(0, L"状态\\字符", LVCFMT_CENTER, 120);
	for (j = 0; j < edgeNumber; j++)
	{
		CString s(edge[j]);
		ShowDate.InsertColumn(j + 1, s, LVCFMT_CENTER, 120);
	}
	CString str;
	for (i = 0; i < DtranNumber; i++)
	{
		str.Format(_T("%d"), i);
		ShowDate.InsertItem(i, str);

		for (j = 0; j < edgeNumber; j++)
		{
			if (DFATable->GetValue(i, j) < 0)
			{
				ShowDate.SetItemText(i, 1 + j, L"");
			}
			else
			{
				str.Format(_T("%d"), DFATable->GetValue(i, j));
				ShowDate.SetItemText(i, 1 + j, str);
			}
		}
	}
}


void CXLexGeneratorDlg::OnBnClickedPrograme()
{
	// TODO: 在此添加控件通知处理程序代码
	Regexp.GetWindowTextW(nowRegexp);
	if (nowRegexp != preRegexp || !hasMinDFA)    //之前没生成过，并且当前正则和之前不一样，则生成
	{
		init();
		// 析构之前的DStates和Dtran表以及AcceptStates数组
		if (DStates != NULL || Dtran != NULL || AcceptStates != NULL)
		{
			for (int i = 0; i < NFAStatesNumber + 1; i++)
			{
				if (DStates != NULL) delete[] DStates[i];
				if (Dtran != NULL) delete[] Dtran[i];
			}
			if (DStates != NULL) delete[] DStates;
			if (Dtran != NULL) delete[] Dtran;
			if (AcceptStates != NULL) delete[] AcceptStates;
		}

		preRegexp = nowRegexp;
		hasMinDFA = true;
		GetRegExp();
		InsertCatNode();
		RegExpToPost();
		GetEdgeNumber();
		ThompsonConstruction();
		SubsetConstruction();
	}
	int i, j;
	// 反复检测Dtran表中是否能化简，能则继续化简
	int DtranNumberAfterMinimization = MinimizeDFAStates(Dtran, AcceptStates, DtranNumber, edgeNumber);
	while (DtranNumberAfterMinimization != DtranNumber)
	{
		DtranNumber = DtranNumberAfterMinimization;
		DtranNumberAfterMinimization = MinimizeDFAStates(Dtran, AcceptStates, DtranNumber, edgeNumber);
	}
	// 将Dtran表的内容拷贝到DFA表
	DFATable = new TransitionTable(DtranNumber, edgeNumber);
	for (i = 0; i < DtranNumber; i++)
	{
		for (j = 0; j < edgeNumber; j++)
		{
			DFATable->SetValue(i, j, Dtran[i][j]);
		}
	}

	//生成词法分析程序
	CString text=L"#include<iostream> \r\nusing namespace std; \r\n";
	
	CString s(edge[0]);
	text += L"char edge[]={ '"+s+L"'";
	for (int j = 1; j < edgeNumber; j++)
	{
		CString s(edge[j]);
		text += L" , '"+s + L"'";
	}
	text += + L" }; \r\n";
		
	CString num1, num2;
	num1.Format(_T("%d"), DtranNumber);
	num2.Format(_T("%d"), edgeNumber);
	text += L"int DFA[" + num1 + L"][" + num2 + L"]={";

	CString str=L"";
	for (int i = 0; i < DtranNumber; i++)
	{
		str+= L"{";
		for (int j = 0; j < edgeNumber; j++)
		{
			if (DFATable->GetValue(i, j) < 0)
			{
				if(j!= edgeNumber-1) num1.Format(_T("%d,"), -1);
				else num1.Format(_T("%d"), -1);
				str += num1;
			}
			else
			{
				if (j != edgeNumber - 1) num1.Format(_T("%d,"), DFATable->GetValue(i, j));
				else num1.Format(_T("%d"), DFATable->GetValue(i, j));
				str += num1;
			}
		}
		if(i!= DtranNumber-1) str += L"},";
		else  str += L"}";
	}
	str += L"}; \r\n";
	text += str;
	num1.Format(_T("%d"), DtranNumber);
	num2.Format(_T("%d"), edgeNumber);
	text += L"int getPos(char c) \r\n { \r\n int i = 0, pos = -1; \r\n for (i = 0; i < " + num2 + L"; i++) \r\n ";
	text += L"{ \r\n if (edge[i] == c) pos = i; \r\n } \r\n  return pos; \r\n } \r\n ";
	text += L"\r\n int main() \r\n { \r\n int j = 0, start = 0, column; \r\n";
	text += L"char *str = new char[256]; \r\n cin >> str; \r\n while (str[j] != \'\\0\') \r\n{ \r\n switch (str[j]) \r\n { \r\n";
	CString case1 = L"",switchstam = L"";
	for (int j = 0; j < edgeNumber; j++)
	{
		case1 = L"";
		CString s(edge[j]);
		case1 = L"case '" + s + L"':\r\n column = getPos(str[j]); \r\n if (column != -1 && start!=-1) { \r\n start = DFA[start][column]; \r\n }";
		case1 += L"\r\n else { \r\n cout << \"NO\"<<endl; \r\n return -1; \r\n } \r\n break; \r\n";
		switchstam += case1;
	}
	switchstam += L"default: \r\n cout << \"NO\"<<endl; \r\n return -1; \r\n break; \r\n } \r\n j++; \r\n }";
	text += switchstam;
	text += L"\r\n cout<<\"YES\"<<endl; \r\n return 0; \r\n}";
	
	ShowLEX dlg;
	dlg.setText(text);
	dlg.DoModal();
	//MessageBox(_T("生成成功，请到文件夹查看文件"));
}


// 获取输入
void CXLexGeneratorDlg::GetRegExp()
{
	USES_CONVERSION;
	exp = T2A(nowRegexp.GetBuffer(0)); 
	for (int i = 0; exp[i] != '\0'; i++)
	{
		if (exp[i] == '~')
		{
			MessageBox(_T("\n字符'~'已经被程序禁用！"));
		}
	}
}
// 使用点号表示连结点
void CXLexGeneratorDlg::InsertCatNode()
{
	USES_CONVERSION;
	exp = T2A(nowRegexp.GetBuffer(0));
	int i = 0, j, len = nowRegexp.GetLength();
	while (exp[i + 1] != '\0')
	{
		if ( ((exp[i] != '(' && exp[i] != '.' && exp[i] != '|')  || exp[i] == ')' || exp[i] == '*')
			&& (exp[i + 1] != ')' && exp[i + 1] != '.' && exp[i + 1] != '|' && exp[i + 1] != '*') 
		   )
		{
			for (j = len; j > i + 1; j--)
			{
				exp[j] = exp[j - 1];
			}
			exp[i + 1] = '.';
			len++;
			exp[len] = '\0';
			i++;
		}
		i++;
	}
	CString strData(exp);
	nowRegexp = strData;

	//MessageBox(nowRegexp);
}
// 定义运算符的优先级
int CXLexGeneratorDlg::Precedence(char symbol)
{
	int priority;
	switch (symbol)
	{
	case '|': priority = 1; break;
	case '.': priority = 2; break;
	case '*': priority = 3; break;
	default:  priority = 0; break;
	}
	return priority;
}
// 将正则式转为逆波兰式
void CXLexGeneratorDlg::RegExpToPost()
{
	USES_CONVERSION;
	exp = T2A(nowRegexp.GetBuffer(0));
	int i = 0, j = 0;
	char ch, cl;
	strcpy(post, "\0");
	LStack<char> *ls = new LStack<char>();
	ls->Clear();
	ls->Push('#');
	ch = exp[i];

	while (ch != '\0')
	{
		if (ch == '(')
		{
			ls->Push(ch);
			ch = exp[++i];
		}
		else if (ch == ')')
		{
			while (ls->GetTop() != '(')
			{
				post[j++] = ls->Pop();
			}
			ls->Pop();
			ch = exp[++i];
		}
		else if ((ch == '|') || (ch == '*') || (ch == '.'))
		{
			cl = ls->GetTop();
			while (Precedence(cl) >= Precedence(ch))
			{
				post[j++] = cl;
				ls->Pop();
				cl = ls->GetTop();
			}
			ls->Push(ch);
			ch = exp[++i];
		}
		else
		{
			post[j++] = ch;
			ch = exp[++i];
		}
	}
	ch = ls->Pop();
	while ((ch == '|') || (ch == '*') || (ch == '.'))
	{
		post[j++] = ch;
		ch = ls->Pop();
	}

	post[j] = '\0';
	ls->Clear();
	CString temp(post);
	//MessageBox(temp);
}
// 扫描逆波兰式中除运算符以外的字符的数目
void CXLexGeneratorDlg::GetEdgeNumber()
{
	int i = 0, j;
	edgeNumber = 0;
	while (post[i] != '\0')
	{
		if (post[i] == '.' || post[i] == '|' || post[i] == '*')
		{
			i++;
			continue;
		}
		for (j = 0; j < edgeNumber; j++)
		{
			if (post[i] == edge[j])   //如果该字符已经存在了，结束
			{
				break;
			}
		}
		if (j == edgeNumber)
		{
			edge[edgeNumber] = post[i];
			edgeNumber++;
		}
		i++;
	}
	edge[edgeNumber] = '\0';
}
// 用Thompson构造法构造NFA
void CXLexGeneratorDlg::ThompsonConstruction()
{
	int i, j;
	char ch;
	int s1, s2;
	LStack<int> *states = new LStack<int>();
	states->Clear();
	if (strlen(post) < 1)
	{
		MessageBox(L"No Valid Regular Expression Found!");
		exit(1);
	}
	NFATable->SetValue(0, 0);
	i = 1;
	j = 0;
	ch = post[j];
	while (ch != '\0')
	{
		if (ch == '.')
		{
			s2 = states->Pop();
			int temp1 = states->Pop();
			int temp2 = states->Pop();
			s1 = states->Pop();
			NFATable->InsertEdgeByValue(temp2, temp1, '~');
			states->Push(s1);
			states->Push(s2);
		}
		else if (ch == '|')
		{
			s2 = states->Pop();
			int temp1 = states->Pop();
			int temp2 = states->Pop();
			s1 = states->Pop();
			NFATable->InsertVertex(i);
			NFATable->InsertVertex(i + 1);
			NFATable->InsertEdgeByValue(i, s1, '~');
			NFATable->InsertEdgeByValue(i, temp1, '~');
			NFATable->InsertEdgeByValue(temp2, i + 1, '~');
			NFATable->InsertEdgeByValue(s2, i + 1, '~');
			s1 = i;
			s2 = i + 1;
			states->Push(s1);
			states->Push(s2);
			i += 2;
		}
		else if (ch == '*')
		{
			s2 = states->Pop();
			s1 = states->Pop();
			NFATable->InsertVertex(i);
			NFATable->InsertVertex(i + 1);
			NFATable->InsertEdgeByValue(i, i + 1, '~');
			NFATable->InsertEdgeByValue(s2, s1, '~');
			NFATable->InsertEdgeByValue(i, s1, '~');
			NFATable->InsertEdgeByValue(s2, i + 1, '~');
			s1 = i;
			s2 = i + 1;
			states->Push(s1);
			states->Push(s2);
			i += 2;
		}
		else
		{
			NFATable->InsertVertex(i);
			NFATable->InsertVertex(i + 1);
			NFATable->InsertEdgeByValue(i, i + 1, ch);
			s1 = i;
			s2 = i + 1;			
			states->Push(s1);
			states->Push(s2);
			i += 2;
			
		}
		j++;
		ch = post[j];
	}
	s2 = states->Pop();
	s1 = states->Pop();
	NFATable->InsertEdgeByValue(0, s1, '~');
	if (!states->IsEmpty())
	{
		MessageBox(L"Some error in your input string!");
		exit(1);
	}
	NFAStatesNumber = s2 + 1;
}
// 比较两个一维数组是否含有完全相同的元素
int CXLexGeneratorDlg::CompArray(int *t1, int *t2)
{
	int i = 0, j = 0, len1, len2;
	while (t1[i] != -1)
	{
		i++;
	}
	len1 = i;
	while (t2[j] != -1)
	{
		j++;
	}
	len2 = j;
	if (len1 != len2)
	{
		return 0;
	}
	for (i = 0; i < len1; i++)
	{
		for (j = 0; j < len2; j++)
		{
			if (t1[i] == t2[j])
			{
				break;
			}
		}
		if (j == len2)
		{
			return 0;
		}
	}
	return 1;
}
// 最小化Dtran表
int CXLexGeneratorDlg::MinimizeDFAStates(int **Dtran, int *AcceptStates, int DtranNumber, int edgeNumber)
{
	int h, i, j, k, l;
	for (i = 0; i < DtranNumber - 1; i++)
	{
		for (j = i + 1; j < DtranNumber; j++)
		{
			if (AcceptStates[i] == AcceptStates[j])
			{
				for (k = 0; k < edgeNumber; k++)
				{
					if (Dtran[i][k] != Dtran[j][k])
					{
						break;
					}
				}
				if (k == edgeNumber)
				{
					for (l = j; l < DtranNumber - 1; l++)
					{
						for (k = 0; k < edgeNumber; k++)
						{
							Dtran[l][k] = Dtran[l + 1][k];
						}
						AcceptStates[l] = AcceptStates[l + 1];
					}
					for (l = 0; l < DtranNumber - 1; l++)
					{
						for (k = 0; k < edgeNumber; k++)
						{
							if (Dtran[l][k] == j)
							{
								Dtran[l][k] = i;
							}
						}
					}
					for (h = j; h < DtranNumber; h++)
					{
						for (l = 0; l < DtranNumber - 1; l++)
						{
							for (k = 0; k < edgeNumber; k++)
							{
								if (Dtran[l][k] == h + 1)
								{
									Dtran[l][k] = h;
								}
							}
						}
					}
					DtranNumber--;
					j--;
				}
			}
		}
	}
	return DtranNumber;
}
// 用子集构造法构造DFA
void CXLexGeneratorDlg::SubsetConstruction()
{
	int i, j, k;
	DStatesNumber = 0;
	DtranNumber = 0;
	// 初始化DStates和Dtran两个构造表及AcceptStates数组
	DStates = (int**)(new int*[NFAStatesNumber + 1]);
	for (i = 0; i < NFAStatesNumber + 1; i++)
	{
		DStates[i] = new int[NFAStatesNumber + 1];
	}

	Dtran = (int**)(new int*[NFAStatesNumber + 1]);
	for (i = 0; i < NFAStatesNumber + 1; i++)
	{
		Dtran[i] = new int[edgeNumber + 1];   //edgeNumber是非终结符的数量
	}

	for (i = 0; i < NFAStatesNumber + 1; i++)
	{
		for (j = 0; j < edgeNumber + 1; j++)
		{
			Dtran[i][j] = -1;
		}
	}

	AcceptStates = new int[NFAStatesNumber + 1];
	for (i = 0; i < NFAStatesNumber + 1; i++)
	{
		AcceptStates[i] = 0;
	}

	// 调用闭包函数和移动函数构造DStates和Dtran两个表及AcceptStates数组
	int *T = new int[NFAStatesNumber + 1];
	int *temp = new int[NFAStatesNumber + 1];
	T[0] = 0;
	T[1] = -1;
	T = NFATable->Closure(T);
	DStates[DStatesNumber] = T;
	DStatesNumber++;
	k = 0;
	while (k < DStatesNumber)
	{
		for (i = 0; edge[i] != '\0'; i++)
		{
			temp = NFATable->Closure(NFATable->Move(T, edge[i]));
			if (temp[0] != -1)
			{
				for (j = 0; j < DStatesNumber; j++)
				{
					if (CompArray(temp, DStates[j]) == 1)
					{
						Dtran[k][i] = j;
						break;
					}
				}
				if (j == DStatesNumber)
				{
					DStates[DStatesNumber] = temp;
					Dtran[k][i] = DStatesNumber;
					DStatesNumber++;
				}
			}
		}
		k++;
		T = DStates[k];
	}
	DtranNumber = k;
	for (i = 0; i < DStatesNumber; i++)
	{
		for (j = 0; DStates[i][j] != -1; j++)
		{
			if (DStates[i][j] == NFAStatesNumber - 1)
			{
				AcceptStates[i] = 1;
				break;
			}
		}
	}
}
void CXLexGeneratorDlg::init()
{
	hasNFA = false;
	hasDFA = false;
	hasMinDFA = false;
	hasPrograme = false;
	exp = new char[128];
	post = new char[128];
	edge = new char[128];
	edgeNumber = 0;
	NFAStatesNumber = 0;
	DFAStatesNumber = 0;
	DStatesNumber = 0;
	DtranNumber = 0;
	NFATable = new Graph();
}

void CXLexGeneratorDlg::OnBnClickedOpen()
{
	// TODO: 在此添加控件通知处理程序代码 
	CString path;
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		path = dlg.GetPathName();
		CStdioFile file;
		CString text, line;
		file.Open(path, CFile::modeRead);
		while (file.ReadString(line))
		{
			text += line;
		}
		file.Close();
		SetDlgItemText(IDC_REGEXP, text);
	}
	else
	{
		MessageBox(L"文件选择窗体创建失败");
		return;
	}
}


void CXLexGeneratorDlg::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CString text,path, fileName=L"temp.txt";
	CFileDialog dlg(FALSE);   
	dlg.m_ofn.lpstrTitle = L"我的文件保存对话框";
	dlg.m_ofn.lpstrFilter = L"Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrDefExt = L"txt";
	if (IDOK == dlg.DoModal())
	{
		path = dlg.GetPathName();
		fileName = dlg.GetFileName();
		Regexp.GetWindowTextW(text);
		//创建文件
		CStdioFile file;
		file.Open(path, CFile::modeCreate | CFile::modeWrite);
		file.SeekToEnd();
		file.WriteString(text);
		file.Flush();
		//关闭文件
		file.Close();
	}
}
