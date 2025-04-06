#pragma once

#include "resource.h"

BOOL bRegistrationPage = FALSE;
BOOL bLoginPage = FALSE;
BOOL bForgotPwdPage = FALSE;
BOOL bDeviceListPage = FALSE;
BOOL bBlackListPage = FALSE;

BOOL bUserRegistrationDone = FALSE;
BOOL bUserLoginDone = FALSE;

void ProcessCommand(HWND, WPARAM, LPARAM);

void AddMenu(HWND);
void CreateDialogControls(HWND);
void CreateRegistrationDialogControls(HWND);
void CreateLoginDialogControls(HWND);
void CreateForgotPwdControls(HWND);

void ShowHidePage(HWND);
void ShowHideRegistrationPage(HWND);
void ShowHideLoginPage(HWND);
void ShowHideForgotPwdPage(HWND);

void RegisterUSBNotification(void);
void FetchUSBDeviceDetails(void);

BOOL CheckForRegisteredUser();
int ValidateEmail(char*);

// In DeviceDetector.h
typedef struct {
    char friendlyName[MAX_PATH];
    char manufacturer[MAX_PATH];
    char deviceDescription[MAX_PATH];
    char classGuid[MAX_PATH];
} DeviceInfo;

