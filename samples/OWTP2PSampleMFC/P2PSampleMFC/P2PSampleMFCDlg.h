// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#pragma once

#include "owt/base/deviceutils.h"
#include "owt/base/localcamerastreamparameters.h"
#include "owt/base/videorendererinterface.h"
#include "owt/base/stream.h"
#include "owt/p2p/p2pclient.h"
#include "owt/p2p/p2ppublication.h"
#include "p2psocketsignalingchannel.h"

using namespace owt::base;
using namespace owt::p2p;

// CP2PSampleMFCDlg dialog
class CP2PSampleMFCDlg : public CDialogEx, public P2PClientObserver
{
// Construction
public:
    CP2PSampleMFCDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_P2PSAMPLEMFC_DIALOG };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedConnectButton();
	afx_msg void OnBnClickedRemoteButton();
	afx_msg void OnBnClickedPublishButton();
	afx_msg void OnBnClickedStopButton();
	afx_msg void OnBnClickedDisconnectButton();
	afx_msg void OnBnClickedMessageButton();

	//P2PClient observer callbacks
	void OnChatStopped(const std::string& remote_user_id);
	void OnChatStarted(const std::string& remote_user_id);
	void OnStreamAdded(std::shared_ptr<RemoteStream> stream);
	void OnStreamRemoved(std::shared_ptr<RemoteStream> stream);
	void OnDataReceived(const std::string& remote_user_id,
		                const std::string message);

    P2PClientConfiguration configuration_;
    std::shared_ptr<P2PClient> pc_;
    std::shared_ptr<P2PSocketSignalingChannel> signaling_channel_;
    std::shared_ptr<LocalDesktopStreamParameters> ldsp_;
    std::shared_ptr<LocalStream> local_stream_;
    std::shared_ptr<RemoteStream> remote_stream_;
    std::shared_ptr<P2PPublication> publication_;

    CString local_peer_id_;
    CString url;
    CString video_codec_;
    CString audio_codec_;
    std::string remote_peer_id_;
    bool chat_started_;
    VideoRenderWindow render_window_;
    afx_msg void OnBnClickedUnpublishButton();

    VideoCodec GetVideoCodec(CString& video_codec);
    AudioCodec GetAudioCodec(CString& audio_codec);
    afx_msg void OnBnClickedStatsButton();
};
