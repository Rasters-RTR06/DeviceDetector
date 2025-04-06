#pragma once

#include "resource.h"

BOOL bRegistrationPage = FALSE;
BOOL bLoginPage = FALSE;
BOOL bForgotPwdPage = FALSE;
BOOL bDeviceListPage = FALSE;
BOOL bBlackListPage = FALSE;



void AddMenu(HWND);

void CreateDialogControls(HWND);

void ShowHidePage(HWND);
