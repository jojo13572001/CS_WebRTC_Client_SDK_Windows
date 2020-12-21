// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#include "stdafx.h"
#include "P2PSampleMFC.h"
#include "RemotePeer.h"
#include "afxdialogex.h"


// CRemotePeer dialog

IMPLEMENT_DYNAMIC(CRemotePeer, CDialogEx)

CRemotePeer::CRemotePeer(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG3, pParent)
{

}

CRemotePeer::~CRemotePeer()
{
}

void CRemotePeer::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT2, m_edit_peer);
}


BEGIN_MESSAGE_MAP(CRemotePeer, CDialogEx)
END_MESSAGE_MAP()


// CRemotePeer message handlers


BOOL CRemotePeer::DestroyWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    GetDlgItemText(IDC_EDIT2, id_);
    return CDialogEx::DestroyWindow();
}


BOOL CRemotePeer::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
    SetDlgItemText(IDC_EDIT2, id_);
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
