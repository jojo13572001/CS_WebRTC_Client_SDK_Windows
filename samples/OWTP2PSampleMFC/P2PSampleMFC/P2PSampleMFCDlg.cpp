// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2
#include "stdafx.h"
#include "P2PSampleMFC.h"
#include "P2PSampleMFCDlg.h"
#include "afxdialogex.h"
#include "PeerServer.h"
#include "RemotePeer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CP2PSampleMFCDlg dialog



CP2PSampleMFCDlg::CP2PSampleMFCDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_P2PSAMPLEMFC_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CP2PSampleMFCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CP2PSampleMFCDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CP2PSampleMFCDlg::OnBnClickedConnectButton)
    ON_BN_CLICKED(IDC_REMOTE_BUTTON, &CP2PSampleMFCDlg::OnBnClickedRemoteButton)
    ON_BN_CLICKED(IDC_PUBLISH_BUTTON, &CP2PSampleMFCDlg::OnBnClickedPublishButton)
    ON_BN_CLICKED(IDC_STOP_BUTTON, &CP2PSampleMFCDlg::OnBnClickedStopButton)
    ON_BN_CLICKED(IDC_DISCONNECT_BUTTON, &CP2PSampleMFCDlg::OnBnClickedDisconnectButton)
    ON_BN_CLICKED(IDC_MESSAGE_BUTTON, &CP2PSampleMFCDlg::OnBnClickedMessageButton)
	ON_BN_CLICKED(IDC_UNPUBLISH_BUTTON, &CP2PSampleMFCDlg::OnBnClickedUnpublishButton)
  ON_BN_CLICKED(IDC_STATS_BUTTON, &CP2PSampleMFCDlg::OnBnClickedStatsButton)
END_MESSAGE_MAP()


// CP2PSampleMFCDlg message handlers
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


BOOL CP2PSampleMFCDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here
    //Create sub-window for remote video rendering
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CP2PSampleMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CP2PSampleMFCDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CP2PSampleMFCDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CP2PSampleMFCDlg::OnChatStopped(const std::string& remote_user_id) {

}

void CP2PSampleMFCDlg::OnChatStarted(const std::string& remote_user_id) {

}

void CP2PSampleMFCDlg::OnStreamAdded(std::shared_ptr<RemoteStream> stream) {
    remote_stream_ = stream;
	remote_stream_->AttachVideoRenderer(render_window_);
}

void CP2PSampleMFCDlg::OnStreamRemoved(std::shared_ptr<RemoteStream> stream) {

}

void CP2PSampleMFCDlg::OnDataReceived(const std::string& remote_user_id,
	                                    const std::string message) {
	CA2T msg(message.c_str());
	LPCTSTR pcstr = msg;
	MessageBox(pcstr, _T("Tips"), MB_ICONINFORMATION);
}

void CP2PSampleMFCDlg::OnBnClickedConnectButton()
{
	CWnd* video_window = GetDlgItem(IDC_VIDEO_RENDERER);
	HWND video_handle = video_window->GetSafeHwnd();
	CPeerServer peerServerDlg;
	//peerServerDlg.SetUrlValue(L"https://mrtc.myviewboard.cloud");
	peerServerDlg.SetUrlValue(L"https://localhost:8096");
	peerServerDlg.SetIdValue(L"2");
	if (peerServerDlg.DoModal() == IDOK) {
		//Connect to the server.
		if (signaling_channel_.get() == nullptr) {
			signaling_channel_.reset(new P2PSocketSignalingChannel());
		}
		GlobalConfiguration::SetVideoHardwareAccelerationEnabled(true);
		video_codec_ = peerServerDlg.GetVideoCodec();
		audio_codec_ = peerServerDlg.GetAudioCodec();

		VideoCodecParameters video_codec_params;
		video_codec_params.name = GetVideoCodec(video_codec_);
		VideoEncodingParameters video_params(video_codec_params, 0, true);
		configuration_.video_encodings.push_back(video_params);
		AudioCodecParameters audio_codec_params;
		audio_codec_params.name = GetAudioCodec(audio_codec_);
		AudioEncodingParameters audio_params(audio_codec_params, 0);
		configuration_.audio_encodings.push_back(audio_params);

		// Set the render window handle that will be used by the renderer attached to streams.
		render_window_.SetWindowHandle(video_handle);

		// Show the session information
		CString codec_info(L"This client configured ");
		codec_info.Append(L"video codec: " + video_codec_);
		codec_info.Append(L" and audio codec: " + audio_codec_);
		GetDlgItem(IDC_STATUS_STATIC)->SetWindowText(codec_info);
		
		url = peerServerDlg.GetUrlValue();
		local_peer_id_ = peerServerDlg.GetIdValue();
		if (pc_.get() == nullptr) {
			pc_.reset(new P2PClient(configuration_, signaling_channel_));
		}
		pc_->AddObserver(*this);

		CT2CA pszConvertedHost(url);
		std::string host(pszConvertedHost);
		CT2CA pszConvertedToken(local_peer_id_);
		std::string token(pszConvertedToken);
		pc_->Connect(host, token, nullptr, nullptr);
	}
}

void CP2PSampleMFCDlg::OnBnClickedRemoteButton()
{
	CRemotePeer peerIDDlg;
	peerIDDlg.SetValue(L"1");
	if (peerIDDlg.DoModal() == IDOK) {
		CString peer_id = peerIDDlg.GetValue();
		CT2CA pszConvertedId(peer_id);
		std::string r_peer(pszConvertedId);
		remote_peer_id_ = r_peer;
		pc_->AddAllowedRemoteId(r_peer);
	}
}

void CP2PSampleMFCDlg::OnBnClickedPublishButton()
{
	if (!local_stream_.get()) {
		ldsp_.reset(new LocalDesktopStreamParameters(false, true));
		ldsp_->SourceType(LocalDesktopStreamParameters::DesktopSourceType::kFullScreen);
		std::unique_ptr<LocalScreenStreamObserver> observer = std::make_unique<LocalScreenStreamObserver>();
		local_stream_ = LocalStream::Create(ldsp_, move(observer));
	}
	pc_->Publish(remote_peer_id_,
               local_stream_,
               [=](std::shared_ptr<P2PPublication> publication) {
                 publication_ = publication;
                 MessageBox(_T("Publishing successfully!"), _T("Tips"), MB_ICONINFORMATION);
               },
               [=](std::unique_ptr<Exception> exception) {
                 MessageBox(_T("Publishing failed!"), _T("Tips"), MB_ICONINFORMATION);
               });
}

void CP2PSampleMFCDlg::OnBnClickedDisconnectButton()
{
	pc_->Disconnect(nullptr, nullptr);
}

void CP2PSampleMFCDlg::OnBnClickedMessageButton()
{
	pc_->Send(remote_peer_id_, "I am Windows C++ application texting",
		[=]() {
		  MessageBox(_T("Text messaging successfully!"), _T("Tips"), MB_ICONINFORMATION);
    }, 
		[=](std::unique_ptr<Exception> exception) {
			MessageBox(_T("Messaging failed!"), _T("Tips"), MB_ICONINFORMATION);
		});
}

void CP2PSampleMFCDlg::OnBnClickedUnpublishButton()
{
	if (publication_) {
		publication_->Stop();
	}
}

void CP2PSampleMFCDlg::OnBnClickedStopButton()
{
	pc_->Stop(remote_peer_id_,
			  [=]() {
			    MessageBox(_T("Peer connection stopped successfully!"),
			  		         _T("Tips"), MB_ICONINFORMATION);
			  },
			  [=](std::unique_ptr<Exception> exception) {
			    MessageBox(_T("Peer connection failed to stop!"),
				   	         _T("Tips"), MB_ICONINFORMATION);
			  });
}

VideoCodec CP2PSampleMFCDlg::GetVideoCodec(CString& video_codec)
{
	if (!video_codec.CompareNoCase(L"VP8"))
		return VideoCodec::kVp8;
	if (!video_codec.CompareNoCase(L"H264"))
		return VideoCodec::kH264;
	if (!video_codec.CompareNoCase(L"VP9"))
		return VideoCodec::kVp9;
	if (!video_codec.CompareNoCase(L"H265"))
		return VideoCodec::kH265;

	return VideoCodec::kUnknown;
}

AudioCodec CP2PSampleMFCDlg::GetAudioCodec(CString& audio_codec)
{
	if (!audio_codec.CompareNoCase(L"OPUS"))
		return AudioCodec::kOpus;
	if (!audio_codec.CompareNoCase(L"PCMU"))
		return AudioCodec::kPcmu;
	if (!audio_codec.CompareNoCase(L"PCMA"))
		return AudioCodec::kPcma;
  if (!audio_codec.CompareNoCase(L"AC3"))
    return AudioCodec::kAc3;
	return AudioCodec::kUnknown;
}


void CP2PSampleMFCDlg::OnBnClickedStatsButton()
{
  if (publication_) {
    publication_->GetStats(
      [=](std::shared_ptr<ConnectionStats> stats) {
        int32_t transmit_bitrate = stats->video_bandwidth_stats.transmit_bitrate;
        MessageBox(_T("Get stats successfully!"), _T("Tips"), MB_ICONINFORMATION);
      },
      [=](std::unique_ptr<Exception> exception) {
        MessageBox(_T("Get stats failed!"), _T("Tips"), MB_ICONINFORMATION);
      });
  }
}
