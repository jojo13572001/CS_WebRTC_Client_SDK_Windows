// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#pragma once
#include "afxwin.h"


// CPeerServer dialog

class CPeerServer : public CDialogEx
{
    DECLARE_DYNAMIC(CPeerServer)

public:
    CPeerServer(CWnd* pParent=NULL);   // standard constructor
    virtual ~CPeerServer();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_PEERSERVER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    CString GetIdValue() const { return id_; }
    void SetIdValue(const CString& id) { id_ = id; }
    CString GetUrlValue() const { return url_; }
    void SetUrlValue(const CString& value) { url_ = value; }
	CString GetVideoCodec() const { return video_codec_; }
	CString GetAudioCodec() const { return audio_codec_; }

private:
    CEdit m_id_edit;
    CEdit m_url_edit;
    CString id_;
    CString url_;
	CString video_codec_;
	CString audio_codec_;
    afx_msg void OnBnClickedOk();
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
public:
	CComboBox m_video_select;
	CComboBox m_audio_select;
};
