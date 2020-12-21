// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#include "stdafx.h"
#include "P2PSampleMFC.h"
#include "PeerServer.h"
#include "afxdialogex.h"


// CPeerServer dialog

IMPLEMENT_DYNAMIC(CPeerServer, CDialogEx)

CPeerServer::CPeerServer(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_PEERSERVER_DIALOG, pParent)
{
}

CPeerServer::~CPeerServer()
{
}

void CPeerServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_URL_EDIT, m_url_edit);
	DDX_Control(pDX, IDC_ID_EDIT, m_id_edit);
	DDX_Control(pDX, IDC_VIDEO_COMBO, m_video_select);
	DDX_Control(pDX, IDC_AUDIO_COMBO, m_audio_select);
}


BEGIN_MESSAGE_MAP(CPeerServer, CDialogEx)
    ON_BN_CLICKED(IDOK, &CPeerServer::OnBnClickedOk)
END_MESSAGE_MAP()


// CPeerServer message handlers


void CPeerServer::OnBnClickedOk()
{
    m_url_edit.GetWindowText(url_);
    m_id_edit.GetWindowText(id_);

	  int nIndex = m_video_select.GetCurSel();
	  m_video_select.GetLBText(nIndex, video_codec_);

	  nIndex = m_audio_select.GetCurSel();
	  m_audio_select.GetLBText(nIndex, audio_codec_);
    CDialogEx::OnOK();
}


BOOL CPeerServer::DestroyWindow()
{
    GetDlgItemText(IDC_URL_EDIT, url_);
    GetDlgItemText(IDC_ID_EDIT, id_);
    return CDialogEx::DestroyWindow();
}


BOOL CPeerServer::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetDlgItemText(IDC_URL_EDIT, url_);
    SetDlgItemText(IDC_ID_EDIT, id_);
	  SetDlgItemText(IDC_VIDEO_COMBO, video_codec_);
	  SetDlgItemText(IDC_AUDIO_COMBO, audio_codec_);

	  m_video_select.SetCurSel(0);
	  m_audio_select.SetCurSel(0);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
