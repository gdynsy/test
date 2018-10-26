// AssetsDlg.cpp : 实现文件

#include "stdafx.h"
#include "DesktopEntry.h"
#include "AssetsDlg.h"
#include "afxdialogex.h"

extern _ConnectionPtr m_pConnection;		//-- 连接对象指针
_RecordsetPtr  m_pRecordsetQ;
// AssetsDlg 对话框

IMPLEMENT_DYNAMIC(AssetsDlg, CDialogEx)

AssetsDlg::AssetsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(AssetsDlg::IDD, pParent)
	, m_typename(_T(""))
	, m_asscode(_T(""))
	, m_aRFID(_T(""))
	, m_aRFID_re(_T(""))
	, m_assname(_T(""))
	, m_QueryType(0)
	, m_EdtQAssets(_T(""))
{

}

AssetsDlg::~AssetsDlg()
{
}

void AssetsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CARD_ASSETS, m_list_card_assets);
	DDX_Control(pDX, IDC_LIST_ASSETS, m_list_assets);
	DDX_Text(pDX, IDC_TypeName, m_typename);
	DDX_Text(pDX, IDC_AssCode, m_asscode);
	DDX_Text(pDX, IDC_ARFID, m_aRFID);
	DDX_Text(pDX, IDC_ARFID_RE, m_aRFID_re);
	DDX_Text(pDX, IDC_AssName, m_assname);
	DDX_CBIndex(pDX, IDC_CobQAssets, m_QueryType);
	DDX_Text(pDX, IDC_EdtQAssets, m_EdtQAssets);
}


BEGIN_MESSAGE_MAP(AssetsDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ASSETS, &AssetsDlg::OnClickListAssets)
	ON_BN_CLICKED(IDC_Modify_Assets, &AssetsDlg::OnBnClickedModifyAssets)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CARD_ASSETS, &AssetsDlg::OnDblclkListCardAssets)
	ON_BN_CLICKED(IDC_RefreshList_assets, &AssetsDlg::OnBnClickedRefreshlistassets)
	ON_BN_CLICKED(IDC_PrePage_a, &AssetsDlg::OnBnClickedPrepagea)
	ON_BN_CLICKED(IDC_NextPage_a, &AssetsDlg::OnBnClickedNextpagea)
	ON_BN_CLICKED(IDC_BtnQAssets, &AssetsDlg::OnBnClickedBtnqassets)
END_MESSAGE_MAP()


// AssetsDlg 消息处理程序

/***********************************************************************************
函数名称：OnInitDialog
功能描述：对话框初始化函数
************************************************************************************/
BOOL AssetsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	pAssetsDlg = this;

	//-- 变量初始化
	m_QueryType = 0;

	//-- 读卡列表初始化
	m_list_card_assets.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_list_card_assets.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	m_list_card_assets.InsertColumn(0, "序号");
	m_list_card_assets.SetColumnWidth(0, 70);
	m_list_card_assets.InsertColumn(1, "RFID");
	m_list_card_assets.SetColumnWidth(1, 257);

	//-- 资产列表初始化
	m_list_assets.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_list_assets.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	m_list_assets.InsertColumn(0, "资产分类");
	m_list_assets.SetColumnWidth(0, 70);
	m_list_assets.InsertColumn(1, "资产编号");
	m_list_assets.SetColumnWidth(1, 70);
	m_list_assets.InsertColumn(2, "资产名称");
	m_list_assets.SetColumnWidth(2, 100);
	m_list_assets.InsertColumn(3, "RFID");
	m_list_assets.SetColumnWidth(3, 190);
	pageSize = 23;	// -- 每页显示条数
	pageNo = 1;		// -- 页数编号
	
	//-- 连接数据库
	int CRet = ConnectionDataBase();
	if (CRet == TRUE)
	{
		m_static = ListState;
		//统计行数页数
		Listcount();
		//刷新资产列表
		Listshow();
	}
	else
	{
		CString str = " 连接数据库错误!";
		MessageBox(str);
	}

	return TRUE;
}

/***********************************************************************************
函数名称：OnClickListAssets
功能描述：单击资产列表
************************************************************************************/
void AssetsDlg::OnClickListAssets(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int m_nRowIndex = pNMListView->iItem;
	if (m_nRowIndex != -1)
	{
		m_typename = m_list_assets.GetItemText(m_nRowIndex, 0);
		m_asscode = m_list_assets.GetItemText(m_nRowIndex, 1);
		m_assname = m_list_assets.GetItemText(m_nRowIndex, 2);
		m_aRFID = m_list_assets.GetItemText(m_nRowIndex, 3);
		UpdateData(FALSE);
	}

	*pResult = 0;
}


/***********************************************************************************
函数名称：OnDblclkListCardAssets
功能描述：双击读卡列表
************************************************************************************/
void AssetsDlg::OnDblclkListCardAssets(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
	int m_nRowIndex = pNMListView->iItem;
	if (m_nRowIndex != -1)
	{
		// 获取被选择列表项第一个子项的文本   
		m_aRFID_re = m_list_card_assets.GetItemText(m_nRowIndex, 1);

		UpdateData(FALSE);
	}

	*pResult = 0;
}

/***********************************************************************************
函数名称：OnBnClickedModifyAssets
功能描述：修改更新数据库
************************************************************************************/
void AssetsDlg::OnBnClickedModifyAssets()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	UpdataAssetsAccount(m_aRFID_re, m_asscode);
	m_aRFID = m_aRFID_re;
	UpdateData(FALSE);
	Listshow();
}


/***********************************************************************************
函数名称：OnBnClickedRefreshlistassets
功能描述：重新刷新资产列表
************************************************************************************/
void AssetsDlg::OnBnClickedRefreshlistassets()
{
	// TODO:  在此添加控件通知处理程序代码
	Listcount();
	Listshow();
}

/***********************************************************************************
函数名称：OnBnClickedPrepagea
功能描述：列表上一页
************************************************************************************/
void AssetsDlg::OnBnClickedPrepagea()
{
	// TODO:  在此添加控件通知处理程序代码
	if (pageNo == 1)
	{
		MessageBox("已经是第一页");
	}
	if (pageNo > 1)
	{
		pageNo--;
		Listshow();
	}
}

/***********************************************************************************
函数名称：OnBnClickedNextpagea
功能描述：列表下一页
************************************************************************************/
void AssetsDlg::OnBnClickedNextpagea()
{
	// TODO:  在此添加控件通知处理程序代码
	if (pageNo == max_page)
	{
		MessageBox("已经是最后一页");
	}
	if (pageNo < max_page)
	{
		pageNo++;
		Listshow();
	}
	
}

/***********************************************************************************
函数名称：Listcount
功能描述：获取数据表行数
************************************************************************************/
void AssetsDlg::Listcount()
{
	_RecordsetPtr  m_pRecordsetT;
	CString str = "";
	int count;	//总行数
	//------------------------ 从数据库中读取数据 ---------------------------
	CString sql = "";

	if (m_static == ListState)
	{
		//查询整个数据表
		sql.Format("select count(*) from assets_input");
	}
	else if (m_static == QueryState)
	{
		//查询关键字	
		sql.Format("select count(*) from assets_type right join assets_input \
					on assets_type.id = assets_input.type_id \
					where %s = '%s'", s_QType, m_EdtQAssets);
	}

	
	try
	{
		m_pRecordsetT = m_pConnection->Execute((_bstr_t)sql, NULL, (long)adCmdText);
	}
	catch (_com_error e)
	{
		CString str = " 连接数据库错误!";
		MessageBox(str);
		return;
	}
	
	if (m_pRecordsetT != NULL)
	{
		str = m_pRecordsetT->GetCollect("");
		count = atoi(str);
	}

	//计算最大页数
	if (count % pageSize == 0)
	{
		max_page = count / pageSize;
	}
	else
	{
		max_page = count / pageSize + 1;
	}
	pageNo = 1;
	m_pRecordsetT->Close();
}

/***********************************************************************************
函数名称：Listshow
功能描述：刷新资产列表
************************************************************************************/
void AssetsDlg::Listshow()
{
	CString str = "";
	int i_num = 0;
	_RecordsetPtr  m_pRecordsetT;

	//--清空列表
	m_list_assets.DeleteAllItems();

	//-- 确定当前页的起始编号
	intStart = pageSize * (pageNo - 1);
	intEnd = pageSize * pageNo;

	//------------------------ 从数据库中读取数据 ---------------------------
	CString sql = "";

	if (m_static == ListState)
	{
		//查询整个数据表
		sql.Format("select assets_type.type_name, p.ass_code, p.ass_name, p.sqbm\
				   	from assets_type right join(\
					select * from(\
					select row_number() over(order by id) as rowid, *from assets_input)t\
					where t.rowid > %d and t.rowid <= %d)p\
					on assets_type.id = p.type_id", intStart, intEnd);
	}
	else if (m_static == QueryState)
	{
		sql.Format("select * from \
						(select row_number() over(order by id) as rowid, *from \
							(select assets_input.id, assets_type.type_name, assets_input.ass_code, assets_input.ass_name, assets_input.sqbm \
							from assets_type right join assets_input \
							on assets_type.id = assets_input.type_id \
							where %s = '%s')p \
						)t \
					where t.rowid > %d and t.rowid <= %d", s_QType, m_EdtQAssets, intStart, intEnd);
	}

	//将查询结果导入m_Recordset数据容器，m_pRecordset内的数据就是执行的结果。
	try
	{
		m_pRecordsetT = m_pConnection->Execute((_bstr_t)sql, NULL, (long)adCmdText);
	}
	catch (_com_error e)
	{
		CString str = " 连接数据库错误!";
		MessageBox(str);
		return;
	}

	if (m_pRecordsetT != NULL)
	{
		//把数据表的数据逐条添加到列表框显示
		while (!m_pRecordsetT->adoEOF)
		{
			//当前页的列表显示
			m_list_assets.InsertItem(i_num, _T(""));

			//-- type_name
			str = m_pRecordsetT->GetCollect("type_name");
			m_list_assets.SetItemText(i_num, 0, str);

			//-- ass_code
			str = m_pRecordsetT->GetCollect("ass_code");
			m_list_assets.SetItemText(i_num, 1, str);

			//-- ass_name
			str = m_pRecordsetT->GetCollect("ass_name");
			m_list_assets.SetItemText(i_num, 2, str);

			//-- RFID
			str = m_pRecordsetT->GetCollect("sqbm");
			m_list_assets.SetItemText(i_num, 3, str);
			
			// 记录游标移到下一条记录
			i_num++;
			m_pRecordsetT->MoveNext();
		}
		m_pRecordsetT->Close();
	}

	//显示页码
	CString strShowPage;
	strShowPage.Format("%d/%d", pageNo, max_page);
	GetDlgItem(IDC_PAGE2)->SetWindowText(strShowPage);
}


/***********************************************************************************
函数名称：OnBnClickedBtnqassets
功能描述：查询按钮
************************************************************************************/
void AssetsDlg::OnBnClickedBtnqassets()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData(TRUE);
	//输入框没有输入时，默认查询整表
	if (m_EdtQAssets == "")
	{
		m_static = ListState;
		//统计行数页数
		Listcount();
		//刷新资产列表
		Listshow();
		return;
	}
	
	//-- 更新当前查询模式
	m_static = QueryState;
	
	pageNo = 1;
	
	CString sql = "";

	//按关键字查询
	switch (m_QueryType)
	{
	case 0://资产分类
		s_QType = "assets_type.type_name";
		break;
	case 1://资产编号
		s_QType = "assets_input.ass_code";
		break;
	case 2://资产名称
		s_QType = "assets_input.ass_name";
		break;
	case 3://RFID
		s_QType = "assets_input.sqbm";
		break;
	default:
		break;
	}
	
	//显示
	Listcount();
	Listshow();
}

/***********************************************************************************
函数名称：PreTranslateMessage
功能描述：屏蔽ESC键与ENTER回车键
************************************************************************************/
BOOL AssetsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	//屏蔽ESC关闭窗体/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
		return TRUE;
	//屏蔽回车关闭窗体,但会导致回车在窗体上失效.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) 
		return TRUE;
	
	return CDialogEx::PreTranslateMessage(pMsg);
}
