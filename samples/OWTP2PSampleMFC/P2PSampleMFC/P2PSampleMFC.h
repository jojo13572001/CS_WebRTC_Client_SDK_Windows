// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"        // main symbols


// CP2PSampleMFCApp:
// See P2PSampleMFC.cpp for the implementation of this class
//

class CP2PSampleMFCApp : public CWinApp
{
public:
    CP2PSampleMFCApp();

// Overrides
public:
    virtual BOOL InitInstance();

// Implementation

    DECLARE_MESSAGE_MAP()
};

extern CP2PSampleMFCApp theApp;