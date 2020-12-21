// ServerURL.cpp : implementation file
//

#include "stdafx.h"
#include "P2PSampleMFC.h"
#include "ServerURL.h"
#include "afxdialogex.h"


// CServerURL dialog

IMPLEMENT_DYNAMIC(CServerURL, CDialogEx)

CServerURL::CServerURL(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG1, pParent)
{    
}

CServerURL::~CServerURL()
{
}

void CServerURL::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CServerURL, CDialogEx)
END_MESSAGE_MAP()


// CServerURL message handlers
