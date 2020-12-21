// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#include "stdafx.h"
#include "ConfSampleMFC.h"
#include "ConfServer.h"
#include "afxdialogex.h"


// CConfServer dialog

IMPLEMENT_DYNAMIC(CConfServer, CDialogEx)

CConfServer::CConfServer(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG2, pParent)
{

}

CConfServer::~CConfServer()
{
}

void CConfServer::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_edit_url);
}


BEGIN_MESSAGE_MAP(CConfServer, CDialogEx)
    ON_BN_CLICKED(IDOK, &CConfServer::OnBnClickedOk)
END_MESSAGE_MAP()


// CConfServer message handlers


void CConfServer::OnBnClickedOk()
{
    m_edit_url.GetWindowText(url_);
    // TODO: Add your control notification handler code here
    CDialogEx::OnOK();
}


BOOL CConfServer::DestroyWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    GetDlgItemText(IDC_EDIT1, url_);
    return CDialogEx::DestroyWindow();
}


BOOL CConfServer::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetDlgItemText(IDC_EDIT1, url_);
    // TODO:  Add extra initialization here

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
