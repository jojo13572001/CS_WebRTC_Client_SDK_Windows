// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#pragma once
#include "afxwin.h"


// CRemotePeer dialog

class CRemotePeer : public CDialogEx
{
    DECLARE_DYNAMIC(CRemotePeer)

public:
    CRemotePeer(CWnd* pParent = NULL);   // standard constructor
    virtual ~CRemotePeer();
    CString GetValue() const { return id_; }
    void SetValue(const CString& value) { id_ = value; }
// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG3 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    CString id_;
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
    CEdit m_edit_peer;
};
