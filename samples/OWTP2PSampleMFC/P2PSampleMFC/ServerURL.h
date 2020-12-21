#pragma once
#include "afxwin.h"


// CServerURL dialog

class CServerURL : public CDialogEx
{
    DECLARE_DYNAMIC(CServerURL)

public:
    CServerURL(CWnd* pParent = NULL);   // standard constructor
    virtual ~CServerURL();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG1 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    CEdit m_Edit;
};
