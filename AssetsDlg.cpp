// AssetsDlg.cpp : ʵ���ļ�

#include "stdafx.h"
#include "DesktopEntry.h"
#include "AssetsDlg.h"
#include "afxdialogex.h"

extern _ConnectionPtr m_pConnection;		//-- ���Ӷ���ָ��
_RecordsetPtr  m_pRecordsetQ;
// AssetsDlg �Ի���

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


// AssetsDlg ��Ϣ�������

/***********************************************************************************
�������ƣ�OnInitDialog
�����������Ի����ʼ������
************************************************************************************/
BOOL AssetsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	pAssetsDlg = this;

	//-- ������ʼ��
	m_QueryType = 0;

	//-- �����б��ʼ��
	m_list_card_assets.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_list_card_assets.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	m_list_card_assets.InsertColumn(0, "���");
	m_list_card_assets.SetColumnWidth(0, 70);
	m_list_card_assets.InsertColumn(1, "RFID");
	m_list_card_assets.SetColumnWidth(1, 257);

	//-- �ʲ��б��ʼ��
	m_list_assets.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_list_assets.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	m_list_assets.InsertColumn(0, "�ʲ�����");
	m_list_assets.SetColumnWidth(0, 70);
	m_list_assets.InsertColumn(1, "�ʲ����");
	m_list_assets.SetColumnWidth(1, 70);
	m_list_assets.InsertColumn(2, "�ʲ�����");
	m_list_assets.SetColumnWidth(2, 100);
	m_list_assets.InsertColumn(3, "RFID");
	m_list_assets.SetColumnWidth(3, 190);
	pageSize = 23;	// -- ÿҳ��ʾ����
	pageNo = 1;		// -- ҳ�����
	
	//-- �������ݿ�
	int CRet = ConnectionDataBase();
	if (CRet == TRUE)
	{
		m_static = ListState;
		//ͳ������ҳ��
		Listcount();
		//ˢ���ʲ��б�
		Listshow();
	}
	else
	{
		CString str = " �������ݿ����!";
		MessageBox(str);
	}

	return TRUE;
}

/***********************************************************************************
�������ƣ�OnClickListAssets
���������������ʲ��б�
************************************************************************************/
void AssetsDlg::OnClickListAssets(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
�������ƣ�OnDblclkListCardAssets
����������˫�������б�
************************************************************************************/
void AssetsDlg::OnDblclkListCardAssets(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
	int m_nRowIndex = pNMListView->iItem;
	if (m_nRowIndex != -1)
	{
		// ��ȡ��ѡ���б����һ��������ı�   
		m_aRFID_re = m_list_card_assets.GetItemText(m_nRowIndex, 1);

		UpdateData(FALSE);
	}

	*pResult = 0;
}

/***********************************************************************************
�������ƣ�OnBnClickedModifyAssets
�����������޸ĸ������ݿ�
************************************************************************************/
void AssetsDlg::OnBnClickedModifyAssets()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	UpdataAssetsAccount(m_aRFID_re, m_asscode);
	m_aRFID = m_aRFID_re;
	UpdateData(FALSE);
	Listshow();
}


/***********************************************************************************
�������ƣ�OnBnClickedRefreshlistassets
��������������ˢ���ʲ��б�
************************************************************************************/
void AssetsDlg::OnBnClickedRefreshlistassets()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	Listcount();
	Listshow();
}

/***********************************************************************************
�������ƣ�OnBnClickedPrepagea
�����������б���һҳ
************************************************************************************/
void AssetsDlg::OnBnClickedPrepagea()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (pageNo == 1)
	{
		MessageBox("�Ѿ��ǵ�һҳ");
	}
	if (pageNo > 1)
	{
		pageNo--;
		Listshow();
	}
}

/***********************************************************************************
�������ƣ�OnBnClickedNextpagea
�����������б���һҳ
************************************************************************************/
void AssetsDlg::OnBnClickedNextpagea()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (pageNo == max_page)
	{
		MessageBox("�Ѿ������һҳ");
	}
	if (pageNo < max_page)
	{
		pageNo++;
		Listshow();
	}
	
}

/***********************************************************************************
�������ƣ�Listcount
������������ȡ���ݱ�����
************************************************************************************/
void AssetsDlg::Listcount()
{
	_RecordsetPtr  m_pRecordsetT;
	CString str = "";
	int count;	//������
	//------------------------ �����ݿ��ж�ȡ���� ---------------------------
	CString sql = "";

	if (m_static == ListState)
	{
		//��ѯ�������ݱ�
		sql.Format("select count(*) from assets_input");
	}
	else if (m_static == QueryState)
	{
		//��ѯ�ؼ���	
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
		CString str = " �������ݿ����!";
		MessageBox(str);
		return;
	}
	
	if (m_pRecordsetT != NULL)
	{
		str = m_pRecordsetT->GetCollect("");
		count = atoi(str);
	}

	//�������ҳ��
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
�������ƣ�Listshow
����������ˢ���ʲ��б�
************************************************************************************/
void AssetsDlg::Listshow()
{
	CString str = "";
	int i_num = 0;
	_RecordsetPtr  m_pRecordsetT;

	//--����б�
	m_list_assets.DeleteAllItems();

	//-- ȷ����ǰҳ����ʼ���
	intStart = pageSize * (pageNo - 1);
	intEnd = pageSize * pageNo;

	//------------------------ �����ݿ��ж�ȡ���� ---------------------------
	CString sql = "";

	if (m_static == ListState)
	{
		//��ѯ�������ݱ�
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

	//����ѯ�������m_Recordset����������m_pRecordset�ڵ����ݾ���ִ�еĽ����
	try
	{
		m_pRecordsetT = m_pConnection->Execute((_bstr_t)sql, NULL, (long)adCmdText);
	}
	catch (_com_error e)
	{
		CString str = " �������ݿ����!";
		MessageBox(str);
		return;
	}

	if (m_pRecordsetT != NULL)
	{
		//�����ݱ������������ӵ��б����ʾ
		while (!m_pRecordsetT->adoEOF)
		{
			//��ǰҳ���б���ʾ
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
			
			// ��¼�α��Ƶ���һ����¼
			i_num++;
			m_pRecordsetT->MoveNext();
		}
		m_pRecordsetT->Close();
	}

	//��ʾҳ��
	CString strShowPage;
	strShowPage.Format("%d/%d", pageNo, max_page);
	GetDlgItem(IDC_PAGE2)->SetWindowText(strShowPage);
}


/***********************************************************************************
�������ƣ�OnBnClickedBtnqassets
������������ѯ��ť
************************************************************************************/
void AssetsDlg::OnBnClickedBtnqassets()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE);
	//�����û������ʱ��Ĭ�ϲ�ѯ����
	if (m_EdtQAssets == "")
	{
		m_static = ListState;
		//ͳ������ҳ��
		Listcount();
		//ˢ���ʲ��б�
		Listshow();
		return;
	}
	
	//-- ���µ�ǰ��ѯģʽ
	m_static = QueryState;
	
	pageNo = 1;
	
	CString sql = "";

	//���ؼ��ֲ�ѯ
	switch (m_QueryType)
	{
	case 0://�ʲ�����
		s_QType = "assets_type.type_name";
		break;
	case 1://�ʲ����
		s_QType = "assets_input.ass_code";
		break;
	case 2://�ʲ�����
		s_QType = "assets_input.ass_name";
		break;
	case 3://RFID
		s_QType = "assets_input.sqbm";
		break;
	default:
		break;
	}
	
	//��ʾ
	Listcount();
	Listshow();
}

/***********************************************************************************
�������ƣ�PreTranslateMessage
��������������ESC����ENTER�س���
************************************************************************************/
BOOL AssetsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	//����ESC�رմ���/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
		return TRUE;
	//���λس��رմ���,���ᵼ�»س��ڴ�����ʧЧ.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) 
		return TRUE;
	
	return CDialogEx::PreTranslateMessage(pMsg);
}
