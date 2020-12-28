// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#include "stdafx.h"
#include "ConfSampleMFC.h"
#include "ConfSampleMFCDlg.h"
#include "afxdialogex.h"
#include "ConfServer.h"

#include "owt/base/localcamerastreamparameters.h"
#include "owt/base/stream.h"
#include "owt/conference/conferenceclient.h"
#include "owt/conference/remotemixedstream.h"
#include "owt/conference/conferencepublication.h"
#include "owt/conference/conferencesubscription.h"
#include "owt/base/globalconfiguration.h"
#include "owt/base/videoencoderinterface.h"
#include "owt/base/logging.h"
#include "EncodedVideoInput.h"
#include "YuvVideoInput.h"
#include "audioframegenerator.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace owt::base;
using namespace owt::conference;
using namespace std;
// CAboutDlg dialog used for App About

using boost::asio::ip::tcp;

namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SSLSocket;

std::vector<std::string>& split(const std::string& s,
  char delim,
  std::vector<std::string>& elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> split(const std::string& s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

// Please create certificate for you MCU host and make sure |verify_peer| is set to true 
string getToken(const string& addr, bool verify_peer) {
  using boost::asio::ip::tcp;
  try {
    boost::asio::io_service io_service;
    boost::asio::ssl::context context(boost::asio::ssl::context::sslv23);
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    vector<string> list = split(addr, '/');
    string server = list[2];
    list = split(server, ':');
    tcp::resolver::query query(list[0], list[1]);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    ssl_socket* socket = new SSLSocket(io_service, context);
    if (verify_peer) {
        socket->set_verify_mode(ssl::verify_peer);
    }
    else {
        socket->set_verify_mode(ssl::verify_none);
    }
    boost::asio::connect(socket->lowest_layer(), endpoint_iterator);
    socket->handshake(SSLSocket::client);

    // Body
    string content =
      "{\"room\":\"\",\"role\":\"presenter\",\"username\":\"11\"}";

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "POST "
      << "/createToken/"
      << " HTTP/1.1\r\n";
    request_stream << "Host: " << list[0] + ":" + list[1] << "\r\n";
    request_stream << "Accept: application/json\r\n";
    request_stream << "Content-Type: application/json\r\n";
    request_stream << "Content-Length: " << content.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream << content;

    // Send the request.
    boost::asio::write(*socket, request);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(*socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        LOG(L"Invalid response\n");
      return "";
    }
    if (status_code != 200) {
        std::wostringstream logStr;
        logStr << L"Response returned with status code " << status_code << endl;
        LOG(logStr.str());
        return "";
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(*socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    std::wostringstream logStr;
    
    LOG(logStr.str());
    while (std::getline(response_stream, header) && header != "\r")
      logStr << std::wstring(header.begin(), header.end()) << endl;
      LOG(logStr.str());
    LOG(L"\n");

    std::ostringstream token_stream;

    // Write whatever content we already have to output.
    if (response.size() > 0) {
      token_stream << &response;
    }

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while (boost::asio::read(*socket, response,
      boost::asio::transfer_at_least(1), error))
      token_stream << &response;
    if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);
    return token_stream.str();
  } catch (std::exception& e) {
      std::wostringstream logStr;
      logStr << L"Exception: " << e.what() << endl;
      LOG(logStr.str());
  }

  return "";
}

// Please create certificate for you MCU host and make sure |verify_peer| is set to true 
void mix(const std::string& addr, const string& room_id, const std::string& pub_id, bool verify_peer) {
  using boost::asio::ip::tcp;
  try {
    boost::asio::io_service io_service;
    boost::asio::ssl::context context(boost::asio::ssl::context::sslv23);
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    vector<string> list = split(addr, '/');
    string server = list[2];
    list = split(server, ':');
    tcp::resolver::query query(list[0], list[1]);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    ssl_socket* socket = new SSLSocket(io_service, context);
    if (verify_peer) {
        socket->set_verify_mode(ssl::verify_peer);
    } else {
        socket->set_verify_mode(ssl::verify_none);
    }
    boost::asio::connect(socket->lowest_layer(), endpoint_iterator);
    socket->handshake(SSLSocket::client);
    // Body
    string content =
      "[{\"op\":\"add\",\"path\":\"/info/inViews\",\"value\":\"common\"}]";

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "PATCH "
      << "/rooms/" << room_id
      << "/streams/" << pub_id
      <<" HTTP/1.1\r\n";
    request_stream << "Host: " << list[0] + ":" + list[1] << "\r\n";
    request_stream << "Accept: application/json\r\n";
    request_stream << "Content-Type: application/json\r\n";
    request_stream << "Content-Length: " << content.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream << content;

    // Send the request.
    boost::asio::write(*socket, request);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(*socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
      LOG(L"Invalid response\n");
      return;
    }
    if (status_code != 200) {
        std::wostringstream logStr;
        logStr << "Response returned with status code " << status_code << endl;
        LOG(logStr.str());
      return;
    }
    return;
  } catch (std::exception& e) {
      std::wostringstream logStr;
      logStr << "Exception: " << e.what() << endl;
      LOG(logStr.str());
  }

  return;
}

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


// CConfSampleMFCDlg dialog



CConfSampleMFCDlg::CConfSampleMFCDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(IDD_CONFSAMPLEMFC_DIALOG, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CConfSampleMFCDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConfSampleMFCDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  //ON_WM_TIMER()
  ON_BN_CLICKED(IDC_BUTTON1, &CConfSampleMFCDlg::OnBnClickedButton1)
  ON_BN_CLICKED(IDC_BUTTON2, &CConfSampleMFCDlg::OnBnClickedButton2)
  ON_BN_CLICKED(IDC_BUTTON4, &CConfSampleMFCDlg::OnBnClickedButton4)
  ON_BN_CLICKED(IDC_BUTTON6, &CConfSampleMFCDlg::OnBnClickedButton6)
  ON_BN_CLICKED(IDC_BUTTON5, &CConfSampleMFCDlg::OnBnClickedButton5)
  ON_WM_SIZE()
END_MESSAGE_MAP()


// CConfSampleMFCDlg message handlers
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}


BOOL CConfSampleMFCDlg::OnInitDialog()
{
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
  GetClientRect(&m_rect);
  m_last_bytes_ = 0;
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CConfSampleMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CConfSampleMFCDlg::OnPaint()
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
HCURSOR CConfSampleMFCDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

static int forward_stream_count = 0;

void CConfSampleMFCDlg::OnStreamAdded(std::shared_ptr<owt::conference::RemoteStream> stream) {
  if (forward_stream_count == 1) {
    forward_stream_count++;
    return;
  }
  remote_forward_stream_ = stream;
  remote_forward_stream_->AddObserver(fwd_observer_);
  SubscribeOptions options;
  VideoCodecParameters codec_params1;
  codec_params1.name = owt::base::VideoCodec::kH264;
  VideoEncodingParameters encoding_params1(codec_params1, 0, false);

  options.video.codecs.push_back(codec_params1);

  room->Subscribe(remote_forward_stream_,
    options,
    [&](std::shared_ptr<ConferenceSubscription> subscription) {
    forward_subscription_ = subscription;
    //forward_subscription_->AddObserver(sub_observer_);
    remote_forward_stream_->AttachVideoRenderer(render_forward_window_);
    LOG(L"Subscribe forward stream succeed\n");
  },
    [=](std::unique_ptr<Exception>) {
    LOG(L"Subscribe forward stream failed\n");
  });
}

void CConfSampleMFCDlg::OnServerDisconnected() {
  SetDlgItemText(IDC_EDIT1, L"Server disconnected.\n");
}


void CConfSampleMFCDlg::OnBnClickedButton1()
{
  CWnd* video_window = GetDlgItem(IDC_STATIC4);
  HWND video_handle = video_window->GetSafeHwnd();
  CWnd* forward_video_window = GetDlgItem(IDC_STATIC9);
  HWND forward_video_handle = forward_video_window->GetSafeHwnd();
  CConfServer serverUrlDlg;
  serverUrlDlg.SetValue(L"https://172.21.4.95:3004/CreateToken");
  if (serverUrlDlg.DoModal() == IDOK) {
    // Connect to the server.
    IceServer ice;
    ice.urls.push_back("stun:stun4.l.google.com:19302");
    ice.username = "";
    ice.password = "";
    std::vector<IceServer> ice_servers;
    ice_servers.push_back(ice);
    configuration_.ice_servers = ice_servers;

	std::unique_ptr<AudioFrameGenerator> audio_frame_generator;
	audio_frame_generator.reset(new AudioFrameGenerator("./audio.pcm", 2, 16000));
	GlobalConfiguration::SetCustomizedAudioInputEnabled(true, std::move(audio_frame_generator));
    // When publishing local encoded stream, this must be enabled. Otherwise disabled.
    //GlobalConfiguration::SetEncodedVideoFrameEnabled(true);

    // Now by default, video hardware acceleration is enabled. On platform prior to Haswell,
    // need to call SetVideoHardwareAccelerationEnabled(false) to disable hardware acceleration.
    GlobalConfiguration::SetVideoHardwareAccelerationEnabled(true);

    //GlobalConfiguration::SetAEC3Enabled(false);

    // Set the render window handle that will be used by the renderer attached to streams  .
    render_window_.SetWindowHandle(video_handle);
    render_forward_window_.SetWindowHandle(forward_video_handle);
    if (room.get() == nullptr) {
      room = ConferenceClient::Create(configuration_);
      room->AddObserver(*this);
    }

    url = serverUrlDlg.GetValue();

    CT2CA pszConvertedStr(url);
    std::string myUrl(pszConvertedStr);

    server_url_ = myUrl;
    std::string token = getToken(myUrl, false);
    if (token != "") {
      std::lock_guard<std::mutex> lock(callback_mutex_);
      room->Join(token,
        [=](std::shared_ptr<ConferenceInfo> info) {
        conference_info_ = info;
        connected_ = true;
        std::vector<std::shared_ptr<RemoteStream>> remote_streams = info->RemoteStreams();
        for (auto& remote_stream : remote_streams) {
          // We only subscribe the first mixed stream. If you would like to subscribe other 
          // mixed stream or forward stream that exists already when joining the room, follow
          // the same approach to subscribe and attach renderer to it.
          if (remote_stream->Source().video == VideoSourceInfo::kMixed) {
            remote_mixed_stream_ = std::static_pointer_cast<RemoteMixedStream>(remote_stream);
            break;
          }
        }
        if (remote_mixed_stream_ != nullptr) {
          auto resoltutions = remote_mixed_stream_->Capabilities().video.resolutions;
          auto bitrates = remote_mixed_stream_->Capabilities().video.bitrate_multipliers;
          auto framerates = remote_mixed_stream_->Capabilities().video.frame_rates;
          SubscribeOptions options;
          for (auto it = resoltutions.begin(); it != resoltutions.end(); it++) {
            options.video.resolution.width = (*it).width;
            options.video.resolution.height = (*it).height;
            break;
          }

          VideoCodecParameters codec_params1;
          codec_params1.name = owt::base::VideoCodec::kH264;
          VideoEncodingParameters encoding_params1(codec_params1, 0, false);
          options.video.codecs.push_back(codec_params1);

          room->Subscribe(remote_mixed_stream_,
            options,
            [&](std::shared_ptr<ConferenceSubscription> subscription) {
            mix_subscription_ = subscription;
            remote_mixed_stream_->AttachVideoRenderer(render_window_);
            SetDlgItemText(IDC_EDIT1, L"Subscribe succeed");
          },
            [=](std::unique_ptr<Exception>) {
            SetDlgItemText(IDC_EDIT1, L"Subscribe failed");
          });
        }
        SetDlgItemText(IDC_EDIT1, L"Join succeeded!");
      },
        [=](unique_ptr<Exception> err) {
        SetDlgItemText(IDC_EDIT1, L"Join failed!");
      }
      );
    }
    else {
        cout << "Create token error!" << endl;
    }
  }
}


void CConfSampleMFCDlg::OnBnClickedButton2()
{
  // TODO: Add your control notification handler code here
}


// The publish button handler
void CConfSampleMFCDlg::OnBnClickedButton4()
{
  if (!connected_)
    return;
  if (!local_stream_.get()) {
    // TODO: Add your control notification handler code here
#if 1  // Case 1: Local camera stream
    lcsp_.reset(new LocalCameraStreamParameters(true, true));
    lcsp_->Resolution(1280, 720);
    int error_code = 0;
    std::vector<std::string> devices = DeviceUtils::VideoCapturerIds();
    if (!devices.empty()) {

      std::string id = devices.at(0);
      lcsp_->CameraId(id);
    }
    local_stream_ = LocalStream::Create(*lcsp_, error_code);
#endif
#if 0 // Case 2: Local yuv stream as customized input.
    std::unique_ptr<FileFrameGenerator> framer(new FileFrameGenerator(640, 480, 30));
    shared_ptr<LocalCustomizedStreamParameters> lcsp(new LocalCustomizedStreamParameters(true, true));
    local_stream_ = LocalStream::Create(lcsp, std::move(framer));
#endif
#if 0 // Case 3: local encoded stream. Provide a video encoder for the stream when it needs encoded data.
    owt::base::VideoEncoderInterface* external_encoder = DirectVideoEncoder::Create(owt::base::VideoCodec::kH264);
    owt::base::Resolution resolution(640, 480);
    shared_ptr<LocalCustomizedStreamParameters> lcsp(new LocalCustomizedStreamParameters(true, true, resolution, 30, 1000));
    local_stream_ = LocalStream::Create(lcsp, external_encoder);
#endif
    // Case 4: local screen sharing stream.
#if 0
    std::unique_ptr<LocalScreenStreamObserver> screen_observer(new LocalScreenObserver());
    lcspscreen_.reset(new LocalDesktopStreamParameters(true, false, LocalDesktopStreamParameters::DesktopSourceType::kApplication));
    local_stream_ = LocalStream::Create(lcspscreen_, std::move(screen_observer));
#endif
  }

  // Publish a stream. If you do not specify PublishOption leave empty 
  // VideoEncodingParameters in the PublishOption.video vector,
  // Client will allow all video types for publication and use default bitrate
  // settings.
  // For non-empty VideoEncodingParameters vector specified, SDK will only
  // allow codecs in the specified list, with the first item in the list preferred.
  PublishOptions options;
  VideoCodecParameters codec_params1;
  codec_params1.name = owt::base::VideoCodec::kVp8;
  VideoEncodingParameters encoding_params1(codec_params1, 0, true);


  VideoCodecParameters codec_params2;
  codec_params2.name = owt::base::VideoCodec::kVp9;
  VideoEncodingParameters encoding_params2(codec_params2, 0, true);

  VideoCodecParameters codec_params3;
  codec_params3.name = owt::base::VideoCodec::kH264;
  VideoEncodingParameters encoding_params3(codec_params3, 4*1024, true);

  //options.video.push_back(encoding_params1);
  //options.video.push_back(encoding_params2);
  options.video.push_back(encoding_params1);

  AudioEncodingParameters audio_params1;
  audio_params1.codec.name = owt::base::AudioCodec::kOpus;
  //options.audio.push_back(audio_params1);
  room->Publish(local_stream_,
    options,
    [=](std::shared_ptr<ConferencePublication> publication) {
    publication_ = publication;
    publication_->AddObserver(pub_observer_);
    std::string pub_id = publication_->Id();
    std::string room_id = conference_info_->Id();
    std::wostringstream logStr;
    logStr << L"publication id is:" << std::wstring(pub_id.begin(), pub_id.end()) << endl;
    LOG(logStr.str());
    mix(server_url_, room_id, pub_id, false);
    SetDlgItemText(IDC_EDIT1, L"publish succeeded!"); 
  },
    [=](unique_ptr<Exception> err) {
      SetDlgItemText(IDC_EDIT1, L"publish failed!");
    });
}

// The unpublish button handler
void CConfSampleMFCDlg::OnBnClickedButton6()
{
  // TODO: Add your control notification handler code here
  if (!connected_) {
    return;
  }
  if (publication_.get()) {
    publication_->Stop();

    publication_.reset();
    publication_ = nullptr;
  }

	  local_stream_.reset();
}


void CConfSampleMFCDlg::OnBnClickedButton5()
{
  // TODO: Add your control notification handler code here
  if (!connected_)
    return;
  local_stream_ = nullptr;

  if (remote_forward_stream_.get()) {
    remote_forward_stream_->RemoveObserver(fwd_observer_);
    remote_forward_stream_->DetachVideoRenderer();
    forward_subscription_->Stop();
  }

  if (remote_mixed_stream_.get()) {
    remote_mixed_stream_->DetachVideoRenderer();
    mix_subscription_->Stop();
  }

  forward_subscription_.reset();
  mix_subscription_.reset();
  forward_stream_count = 0;
  if (publication_) {
	  publication_->Stop();
  }
  room->Leave(nullptr, nullptr);
  connected_ = false;
}


void CConfSampleMFCDlg::OnSize(UINT nType, int cx, int cy)
{
  CDialogEx::OnSize(nType, cx, cy);

  // TODO: Add your message handler code here. 
  // Notify the video static window to maximize itself.    
  CWnd* video_window = GetDlgItem(IDC_STATIC4);
  if (video_window && cx != 0 && cy != 0) {
    HWND video_handle = video_window->GetSafeHwnd();
    CRect rect; //render window size;
    video_window->GetWindowRect(&rect);
    ScreenToClient(&rect);
    if (m_rect.Width() != 0 && m_rect.Height() != 0) {
      rect.left = rect.left*cx / m_rect.Width();
      rect.right = rect.right*cx / m_rect.Width();
      rect.top = rect.top*cy / m_rect.Height();
      rect.bottom = rect.bottom*cy / m_rect.Height() + 8;
      video_window->MoveWindow(rect);
    }
  }
  GetClientRect(&m_rect);
  switch (nType) {
  case SIZE_RESTORED:
    break;
  case SIZE_MINIMIZED:
    break;
  case SIZE_MAXIMIZED:
    break;
  }
}


BOOL CConfSampleMFCDlg::DestroyWindow()
{
  // TODO: Add your specialized code here and/or call the base class
  return CDialogEx::DestroyWindow();
}
