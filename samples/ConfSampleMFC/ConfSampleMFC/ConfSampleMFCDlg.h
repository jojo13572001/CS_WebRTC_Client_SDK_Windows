// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#pragma once

#include "owt/base/localcamerastreamparameters.h"
#include "owt/base/videorendererinterface.h"
#include "owt/base/stream.h"
#include "owt/base/deviceutils.h"
#include "owt/conference/conferenceclient.h"
#include "owt/conference/conferencepublication.h"
#include "owt/conference/conferencesubscription.h"
#include "owt/base/publication.h"
#include <mutex>
#include <unordered_map>
#include <iostream>
#include "afxwin.h"

class LocalScreenObserver : public owt::base::LocalScreenStreamObserver {
    void OnCaptureSourceNeeded(const std::unordered_map<int, std::string>& window_list, int& dest_window) {
        for (auto& r : window_list) {
            if (r.second.find("D3D9") != std::string::npos) {
                dest_window = r.first;
                break;
            }
        }
    }

};

class ConfPubObserver : public owt::base::PublicationObserver {
public:
    void OnEnded() { 
        std::cout << "ended" << std::endl; 
    }

    void OnMute(owt::base::TrackKind track_kind) { 
        std::cout << "on video/audio muted" << std::endl; 
    }

    void OnUnmute(owt::base::TrackKind track_kind) {
        std::cout << "on video/audio unmuted" << std::endl; 
    }

	void OnError(std::unique_ptr<owt::base::Exception> failure) {
		std::cout << "on video/audio error" << std::endl;
	}
};

class ConfSubObserver : public owt::base::SubscriptionObserver {
public:
    void OnEnded() {
        std::cout << "ended" << std::endl;
    }

    void OnMute(owt::base::TrackKind track_kind) {
        std::cout << "on video/audio muted" << std::endl;
    }

    void OnUnmute(owt::base::TrackKind track_kind) {
        std::cout << "on video/audio unmuted" << std::endl;
    }

	void OnError(std::unique_ptr<owt::base::Exception> failure) {
		std::cout << "on video/audio error" << std::endl;
	}

};

class ForwardStreamObserver : public owt::base::StreamObserver {
    void OnUpdated() {
        std::cout << "stream updated" << std::endl;
    }
};


// CConfSampleMFCDlg dialog
class CConfSampleMFCDlg : public CDialogEx, public owt::conference::ConferenceClientObserver/*, public QObject*/
{
// Construction
public:
    CConfSampleMFCDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ConfSAMPLEMFC_DIALOG };
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
    //afx_msg void OnTimer(UINT nIDEvent);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();

public:
    owt::conference::ConferenceClientConfiguration configuration_;
    std::shared_ptr<owt::conference::ConferenceClient> room;
    std::shared_ptr<owt::base::LocalCameraStreamParameters> lcsp_;
    std::shared_ptr<owt::base::LocalCustomizedStreamParameters> lcsp_customized_;
    std::shared_ptr<owt::conference::ConferenceInfo> conference_info_;
    std::shared_ptr<owt::base::LocalStream> local_stream_;
    std::shared_ptr<owt::conference::RemoteMixedStream> remote_mixed_stream_;
    std::shared_ptr<owt::base::RemoteStream> remote_forward_stream_;
    std::shared_ptr<owt::base::LocalDesktopStreamParameters> lcspscreen_;
    std::shared_ptr<owt::conference::ConferencePublication> publication_;
    std::shared_ptr<owt::conference::ConferenceSubscription> mix_subscription_;
    std::shared_ptr<owt::conference::ConferenceSubscription> forward_subscription_;
    mutable std::mutex callback_mutex_;
    owt::base::VideoRenderWindow render_window_;
    owt::base::VideoRenderWindow render_forward_window_;
    ConfPubObserver pub_observer_;
    ConfSubObserver sub_observer_;
    ForwardStreamObserver fwd_observer_;
    CString url;
    std::string server_url_;
    bool connected_;
    CRect m_rect;
    int64_t m_last_bytes_;
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnBnClickedButton6();
    afx_msg void OnBnClickedButton5();

public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void OnStreamAdded(std::shared_ptr<owt::conference::RemoteStream> stream) override;

    void OnServerDisconnected() override;
    virtual BOOL DestroyWindow();
};
