// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#pragma once
#include "afxwin.h"


// CConfServer dialog

class CConfServer : public CDialogEx
{
    DECLARE_DYNAMIC(CConfServer)

public:
	CConfServer(CWnd* pParent=NULL);   // standard constructor
    virtual ~CConfServer();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG2 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    CString GetValue() const { return url_; }
    void SetValue(const CString& value) { url_ = value; }
    CEdit m_edit_url;
    CString url_;
    afx_msg void OnBnClickedOk();
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
};
