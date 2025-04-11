//
#pragma once

#include <windows.h>


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <initguid.h> // For GUID_DEVINTERFACE_USB_DEVICE
#include <Usbiodef.h>
#include <setupapi.h>
#include <devguid.h>
#include <cfgmgr32.h>

// Device detection header file's
#include <dbt.h>
// DEFINE_GUID(GUID_DEVINTERFACE_DISK, 0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);


#include "DeviceDetector.h"
#include "Headers/DeviceDetector_JSON.h"


#define	MAX_HEIGHT	600
#define	MAX_WIDTH	800
#define SPLASH_DURATION 3000

// global function declarations
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    SplashScreenWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AboutWndProc(HWND, UINT, WPARAM, LPARAM);

AdminInfo* gaInfo;

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

void Start_Splash();

BOOL bRegistrationPage = FALSE;
BOOL bLoginPage = FALSE;
BOOL bForgotPwdPage = FALSE;
BOOL bDeviceListPage = FALSE;
BOOL bBlackListPage = FALSE;

BOOL bUserRegistrationDone = FALSE;
BOOL bUserLoginDone = FALSE;

HWND ghwndSplash = NULL;
HDC hSplashDC = NULL;
HDC hMemDC = NULL;
LONG SplashWidth, SplashHeight;



// global variable declarations
HWND ghwnd = NULL;
HMENU ghMenu = NULL;
HBITMAP hSplashBMP;
HINSTANCE ghInstance;

// variable related to File I/O
char gszLogFileName[] = "Log.txt";
FILE* gpFile = NULL;
FILE* gpRegistrationDetailsFile = NULL;

// Entry point funtion
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("RTR6");

	int screenWidth, screenHeight;
	int windowWidthHalf, windowHeightHalf;
	int windowTop, windowLeft;

	// code
	// create log file
	gpFile = fopen(gszLogFileName, "w");

	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Log file creation failed."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Program started successfully\n");
	}

	ghInstance = hInstance;
	Start_Splash();

	// window class Initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// registration of window class
	RegisterClassEx(&wndclass);

	screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	fprintf(gpFile, "screen width = %d\n", screenWidth);
	fprintf(gpFile, "screen height = %d\n", screenHeight);

	windowWidthHalf = MAX_WIDTH / 2;
	windowHeightHalf = MAX_HEIGHT / 2;

	windowLeft = (screenWidth / 2) - windowWidthHalf;
	windowTop = (screenHeight / 2) - windowHeightHalf;

	fprintf(gpFile, "Window Top = %d\n", windowTop);
	fprintf(gpFile, "window Left= %d\n", windowLeft);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("DeviceDetector"),
		WS_OVERLAPPED | \
		WS_CAPTION | \
		WS_SYSMENU | \
		WS_THICKFRAME | \
		WS_MINIMIZEBOX | \
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		windowLeft,
		windowTop,
		MAX_WIDTH,
		MAX_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	RegisterUSBNotification();

	// show window
	ShowWindow(hwnd, iCmdShow);

	// paint background of the window
	UpdateWindow(hwnd);

	// Message Loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// close the log file
	if (gpFile)
	{
		fprintf(gpFile, "Program terminated successfully\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	return((int)msg.wParam);
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//  PURPOSE: Processes messages for the main window.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_GETMINMAXINFO:
	{
		// Fixing Height to 600 & Width to 800
		MINMAXINFO* mm = (MINMAXINFO*)lParam;
		mm->ptMaxSize.x = MAX_WIDTH;
		mm->ptMaxSize.y = MAX_HEIGHT;
		mm->ptMinTrackSize.x = MAX_WIDTH;
		mm->ptMinTrackSize.y = MAX_HEIGHT;
		mm->ptMaxTrackSize.x = MAX_WIDTH;
		mm->ptMaxTrackSize.y = MAX_HEIGHT;
		mm->ptMaxPosition.x = 400;
		mm->ptMaxPosition.y = 300;
		return 0;
	}
	case WM_CREATE:
	{
		AddMenu(hWnd);
		// All Required control will be created in next function
		CreateDialogControls(hWnd);

		if (CheckForRegisteredUser() == TRUE)
		{
			bUserRegistrationDone = TRUE;
			
			bRegistrationPage = FALSE;
			bDeviceListPage = FALSE;
			bForgotPwdPage = FALSE;
			bBlackListPage = FALSE;

			bLoginPage = TRUE;
		}
		else
		{
			bUserRegistrationDone = FALSE;

			bDeviceListPage = FALSE;
			bForgotPwdPage = FALSE;
			bBlackListPage = FALSE;
			bLoginPage = FALSE;

			bRegistrationPage = TRUE;
		}

		ShowHidePage(hWnd);
		break;
	}
    case WM_COMMAND:
	{
		ProcessCommand(hWnd, wParam, lParam);
		break;
	}
	case WM_DEVICECHANGE:
	{
		if (wParam == DBT_DEVICEARRIVAL)
		{
			if (lParam == 0)
				break; // Safety check

			PDEV_BROADCAST_DEVICEINTERFACE lpdb = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
			if (lpdb->dbcc_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			{
				fprintf(gpFile, "USB device Detected successfully...!\n");
				FetchUSBDeviceDetails(); // Ensure this function is implemented correctly
				MessageBox(hWnd, TEXT("Device Arrival!"), TEXT("WM_DEVICECHANGE"), MB_OK | MB_ICONINFORMATION);
			}
		}
		else if (wParam == DBT_DEVICEREMOVECOMPLETE)
		{
			fprintf(gpFile, "USB device removed successfully...!\n");
		}
		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		DWORD ctrlID = GetDlgCtrlID((HWND)lParam);
		if (ctrlID >= IDL_FIRST_NAME || ctrlID <= IDL_FORGOT_PASSWORD)
		{
			HDC hdc = (HDC)wParam;
			//SetBkColor(hdc, (HBRUSH)GetStockObject(GRAY_BRUSH));
			SetBkMode(hdc, TRANSPARENT);
			return (INT_PTR)((HBRUSH)GetStockObject(LTGRAY_BRUSH));
		}
		break;
	}
    case WM_PAINT:
    {
		break;
    }
    case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
    default:
		break;
    }
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// ========== UI Element Creation ==============
#pragma region UI Element Creation
void AddMenu(HWND hWnd)
{
	ghMenu = CreateMenu();

	//File Menu
	HMENU hMenuFile = CreateMenu();
	AppendMenu(hMenuFile, MF_STRING, IDM_FILE_DEVICE_LIST, TEXT("Device List"));
	AppendMenu(hMenuFile, MF_STRING, IDM_FILE_BLACK_LIST,  TEXT("Black List"));
	AppendMenu(hMenuFile, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenuFile, MF_STRING, IDM_FILE_EXIT, TEXT("Exit"));

	AppendMenu(ghMenu, MF_POPUP, (UINT_PTR)hMenuFile, TEXT("File"));

	//Help Menu
	HMENU hMenuHelp = CreateMenu();
	AppendMenu(hMenuHelp, MF_STRING, IDM_HELP_ABOUT, TEXT("About DeviceDetector"));

	AppendMenu(ghMenu, MF_POPUP, (UINT_PTR)hMenuHelp, TEXT("Help"));

	SetMenu(hWnd, ghMenu);
}

void CreateDialogControls(HWND hWnd)
{
	//sample call to create control
	//::CreateWindow(
	//	TEXT("Button"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
	//	TEXT("Login"), //Name on control
	//	WS_CHILD | WS_VISIBLE , //Style reqiured (Explore as required)
	//	100, 100, //left top position WITHIN WINDOW
	//	50, 50, //Size (width n height)
	//	hWnd, // handle of app window
	//	(HMENU)IDD_BUTTON, // ID of button to uniquely identify in othere area of program 
	//	NULL, 
	//	NULL);
	
	//Registration
	CreateRegistrationDialogControls(hWnd);

	//Login
	CreateLoginDialogControls(hWnd);

	//Forgot Password
	CreateForgotPwdControls(hWnd);
}

void CreateRegistrationDialogControls(HWND hWnd)
{
	CreateWindow(TEXT("Static"), TEXT("User Registration"), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		200, 60, 400, 40, hWnd,
		(HMENU)IDL_USER_REGISTRATION, NULL, NULL);
	CreateWindow(TEXT("Static"), TEXT("First Name"), WS_CHILD | WS_VISIBLE,
		250, 155, 100, 25, hWnd,
		(HMENU)IDL_FIRST_NAME, NULL, NULL);
	CreateWindow(TEXT("Static"), TEXT("Last Name"), WS_CHILD | WS_VISIBLE,
		250, 205, 100, 25, hWnd,
		(HMENU)IDL_LAST_NAME, NULL, NULL);
	CreateWindow(TEXT("Static"), TEXT("Email Id"), WS_CHILD | WS_VISIBLE,
		250, 255, 100, 25, hWnd,
		(HMENU)IDL_EMAIL_ID, NULL, NULL);
	CreateWindow(TEXT("Static"), TEXT("Username"), WS_CHILD | WS_VISIBLE,
		250, 305, 100, 25, hWnd,
		(HMENU)IDL_USERNAME, NULL, NULL);
	CreateWindow(TEXT("Static"), TEXT("Password"), WS_CHILD | WS_VISIBLE,
		250, 355, 100, 25, hWnd,
		(HMENU)IDL_PASSWORD, NULL, NULL);

	CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
		400, 150, 200, 25, hWnd,
		(HMENU)IDTB_FIRST_NAME, NULL, NULL);
	CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
		400, 200, 200, 25, hWnd,
		(HMENU)IDTB_LAST_NAME, NULL, NULL);
	CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
		400, 250, 200, 25, hWnd,
		(HMENU)IDTB_EMAIL_ID, NULL, NULL);
	CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
		400, 300, 200, 25, hWnd,
		(HMENU)IDTB_USERNAME, NULL, NULL);
	CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
		400, 350, 200, 25, hWnd,
		(HMENU)IDTB_PASSWORD, NULL, NULL);

	CreateWindow(
		TEXT("BUTTON"), TEXT("Create"), WS_CHILD | WS_VISIBLE,
		325, 420, 150, 40, hWnd,
		(HMENU)IDBTN_CREATE_USER, NULL, NULL);

}

void CreateLoginDialogControls(HWND hWnd)
{
	CreateWindow(TEXT("Static"), TEXT("Login"), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		200, 100, 400, 40, hWnd,
		(HMENU)IDL_USER_LOGIN, NULL, NULL);
	CreateWindow(TEXT("Static"), TEXT("Username"), WS_CHILD | WS_VISIBLE,
		250, 205, 100, 25, hWnd,
		(HMENU)IDL_USERNAME_LOGIN, NULL, NULL);
	CreateWindow(TEXT("Static"), TEXT("Password"), WS_CHILD | WS_VISIBLE,
		250, 255, 100, 25, hWnd,
		(HMENU)IDL_PASSWORD_LOGIN, NULL, NULL);

	CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
		400, 200, 200, 25, hWnd,
		(HMENU)IDTB_USERNAME_LOGIN, NULL, NULL);
	CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
		400, 250, 200, 25, hWnd, 
		(HMENU)IDTB_PASSWORD_LOGIN, NULL, NULL);

	CreateWindow(TEXT("BUTTON"), TEXT("Log In"), WS_CHILD | WS_VISIBLE,
		250, 320, 150, 40, hWnd,
		(HMENU)IDBTN_LOGIN_USER, NULL, NULL);
	CreateWindow(TEXT("BUTTON"), TEXT("Forgot Password"), WS_CHILD | WS_VISIBLE, 
		410, 320, 150, 40, hWnd,
		(HMENU)IDBTN_FORGOT_PASSWORD, NULL, NULL);
}

void CreateForgotPwdControls(HWND hWnd)
{
	CreateWindow(TEXT("Static"), TEXT("Forgot Password"), WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		200, 100, 400, 40, hWnd,
		(HMENU)IDL_FORGOT_PASSWORD, NULL, NULL);
	CreateWindow(TEXT("Static"), TEXT("Email ID"), WS_CHILD | WS_VISIBLE,
		250, 205, 100, 25, hWnd,
		(HMENU)IDL_EMAILID_FGTPWD, NULL, NULL);

	CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,
		400, 200, 200, 25, hWnd,
		(HMENU)IDTB_EMAILID_FGTPWD, NULL, NULL);

	CreateWindow(TEXT("BUTTON"), TEXT("Send"), WS_CHILD | WS_VISIBLE,
		250, 320, 150, 40, hWnd,
		(HMENU)IDBTN_SEND_FGTPWD, NULL, NULL);
	CreateWindow(TEXT("BUTTON"), TEXT("Cancel"), WS_CHILD | WS_VISIBLE,
		410, 320, 150, 40, hWnd,
		(HMENU)IDBTN_CANCEL_FGTPWD, NULL, NULL);
}
#pragma endregion


// ========== Show OR Hide UI Element ==============
#pragma region Show OR Hide UI Element
void ShowHidePage(HWND hWnd)
{
	//Registration Page
	ShowHideRegistrationPage(hWnd);

	//LoginPage
	ShowHideLoginPage(hWnd);
	
	//Forgot Password Page
	ShowHideForgotPwdPage(hWnd);

	//Device List Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bDeviceListPage);

	//Black List Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bBlackListPage);

	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

void ShowHideRegistrationPage(HWND hWnd) 
{
	ShowWindow(GetDlgItem(hWnd, IDL_FIRST_NAME), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDL_LAST_NAME), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDL_EMAIL_ID), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDL_USERNAME), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDL_PASSWORD), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDL_USER_REGISTRATION), bRegistrationPage);

	ShowWindow(GetDlgItem(hWnd, IDTB_FIRST_NAME), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDTB_LAST_NAME), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDTB_EMAIL_ID), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDTB_USERNAME), bRegistrationPage);
	ShowWindow(GetDlgItem(hWnd, IDTB_PASSWORD), bRegistrationPage);
	
	ShowWindow(GetDlgItem(hWnd, IDBTN_CREATE_USER), bRegistrationPage);
}

void ShowHideLoginPage(HWND hWnd)
{
	ShowWindow(GetDlgItem(hWnd, IDL_USERNAME_LOGIN), bLoginPage);
	ShowWindow(GetDlgItem(hWnd, IDL_PASSWORD_LOGIN), bLoginPage);
	ShowWindow(GetDlgItem(hWnd, IDL_USER_LOGIN), bLoginPage);

	ShowWindow(GetDlgItem(hWnd, IDTB_USERNAME_LOGIN), bLoginPage);
	ShowWindow(GetDlgItem(hWnd, IDTB_PASSWORD_LOGIN), bLoginPage);

	ShowWindow(GetDlgItem(hWnd, IDBTN_LOGIN_USER), bLoginPage);
	ShowWindow(GetDlgItem(hWnd, IDBTN_FORGOT_PASSWORD), bLoginPage);
}

void ShowHideForgotPwdPage(HWND hWnd)
{
	ShowWindow(GetDlgItem(hWnd, IDL_EMAILID_FGTPWD), bForgotPwdPage);
	ShowWindow(GetDlgItem(hWnd, IDL_FORGOT_PASSWORD), bForgotPwdPage);

	ShowWindow(GetDlgItem(hWnd, IDTB_EMAILID_FGTPWD), bForgotPwdPage);

	ShowWindow(GetDlgItem(hWnd, IDBTN_SEND_FGTPWD), bForgotPwdPage);
	ShowWindow(GetDlgItem(hWnd, IDBTN_CANCEL_FGTPWD), bForgotPwdPage);
}
#pragma endregion

//============== Processing of Commands =======
#pragma region Processing of Commands
void ProcessCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iControlId = LOWORD(wParam);
	// Parse the selections:
	switch (iControlId)
	{
		//Menu Commands
	case IDM_FILE_DEVICE_LIST:
	{
		if (bUserRegistrationDone == TRUE && bUserLoginDone == TRUE)
		{
			bRegistrationPage = FALSE;
			bLoginPage = FALSE;
			bForgotPwdPage = FALSE;
			bBlackListPage = FALSE;

			bDeviceListPage = TRUE;
			ShowHidePage(hWnd);
		}
		break;
	}
	case IDM_FILE_BLACK_LIST:
	{
		if (bUserRegistrationDone == TRUE && bUserLoginDone == TRUE)
		{
			bRegistrationPage = FALSE;
			bLoginPage = FALSE;
			bForgotPwdPage = FALSE;
			bDeviceListPage = FALSE;

			bBlackListPage = TRUE;
			ShowHidePage(hWnd);
		}
		break;
	}
	case IDM_HELP_ABOUT:
	{
		DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutWndProc);
		break;
	}
	case IDM_FILE_EXIT:
	{
		//Save Json Data to .Dat file
		//Encrypt .Dat file 
		//Close .Dat file
		DestroyWindow(hWnd);
		break;
	}


	//Button Commands
	/*case IDBTN_BUTTON:
	{
		bLoginPage = FALSE;
		bDeviceListPage = TRUE;
		ShowHidePage(hWnd);
		break;
	}*/
	case IDBTN_CREATE_USER:
	{
		//gpRegistrationDetailsFile = fopen("UserRegistrationDetails.txt", "w");

		//if (gpRegistrationDetailsFile == NULL)
		//{
		//	MessageBox(hWnd, TEXT("Registration Details File Creation Failed."), TEXT("File I/O Error"), MB_OK);
		//	bUserRegistrationDone = FALSE;
		//}
		//else
		{
			int inputLength;
			BOOL validPwd = FALSE;
			BOOL validEmail = FALSE;

			inputLength = GetWindowTextLength(GetDlgItem(hWnd, IDTB_FIRST_NAME));
			char* firstName = (char*)malloc(inputLength + 1);
			GetWindowText(GetDlgItem(hWnd, IDTB_FIRST_NAME), firstName, inputLength + 1);

			inputLength = GetWindowTextLength(GetDlgItem(hWnd, IDTB_LAST_NAME));
			char* lastName = (char*)malloc(inputLength + 1);
			GetWindowText(GetDlgItem(hWnd, IDTB_LAST_NAME), lastName, inputLength + 1);

			inputLength = GetWindowTextLength(GetDlgItem(hWnd, IDTB_EMAIL_ID));
			char* emailId = (char*)malloc(inputLength + 1);
			GetWindowText(GetDlgItem(hWnd, IDTB_EMAIL_ID), emailId, inputLength + 1);

			inputLength = GetWindowTextLength(GetDlgItem(hWnd, IDTB_USERNAME));
			char* userName = (char*)malloc(inputLength + 1);
			GetWindowText(GetDlgItem(hWnd, IDTB_USERNAME), userName, inputLength + 1);

			inputLength = GetWindowTextLength(GetDlgItem(hWnd, IDTB_PASSWORD));
			char* password = (char*)malloc(inputLength + 1);
			GetWindowText(GetDlgItem(hWnd, IDTB_PASSWORD), password, inputLength + 1);

			int pwdLen = strlen(password) + 1;

			if (pwdLen >= 9)
			{
				validPwd = TRUE;
			}
			else
			{
				MessageBox(hWnd, TEXT("Password should be of minimum 8 characters."), TEXT("Validation Error"), MB_OK);
			}

			if (ValidateEmail(emailId) == TRUE) 
			{
				validEmail = TRUE;
			}
			else {
				MessageBox(hWnd, TEXT("Please enter valid email ID"), TEXT("Validation Error"), MB_OK);
			}

			if (validEmail == TRUE && validPwd == TRUE)
			{
				bUserRegistrationDone = TRUE;
				gaInfo = (AdminInfo*)malloc(sizeof(AdminInfo));
				memset(gaInfo, 0, sizeof(AdminInfo));
				strcpy(gaInfo->firstName, firstName);
				strcpy(gaInfo->lastName, lastName);
				strcpy(gaInfo->emailID, emailId);
				strcpy(gaInfo->userName, userName);
				strcpy(gaInfo->password, password);

				//fprintf(gpRegistrationDetailsFile, "First Name: %s\nLast Name: %s\nEmail Id: %s\nUser Name: %s\nPassword: %s\n\n", gaInfo->firstName, gaInfo->lastName, gaInfo->emailID, gaInfo->userName, gaInfo->password);
				AddAdminDetails(gaInfo);
			}
			else
			{
				bUserRegistrationDone = FALSE;
			}

			free(firstName);
			free(lastName);
			free(emailId);
			free(userName);
			free(password);
		}
		//fclose(gpRegistrationDetailsFile);
		//gpRegistrationDetailsFile = NULL;

		if (bUserRegistrationDone == TRUE)
		{
			bLoginPage = TRUE;
			bRegistrationPage = FALSE;
			
			bBlackListPage = FALSE;
			bForgotPwdPage = FALSE;
			bDeviceListPage = FALSE;

			ShowHidePage(hWnd);
		}
		break;
	}
	case IDBTN_LOGIN_USER:
	{
		//gpRegistrationDetailsFile = fopen("UserRegistrationDetails.txt", "r");

		if (gaInfo == NULL)
		{
			MessageBox(hWnd, TEXT("Admin Details Not Found."), TEXT("File I/O Error"), MB_OK);
			bUserLoginDone = FALSE;
		}
		else
		{
			int inputLength;
			char line[100];      // Buffer for reading lines from the file
			char userName[50];   // Buffer for storing the username
			char password[50];

			inputLength = GetWindowTextLength(GetDlgItem(hWnd, IDTB_USERNAME_LOGIN));
			char* userNameF = (char*)malloc(inputLength + 1);
			GetWindowText(GetDlgItem(hWnd, IDTB_USERNAME_LOGIN), userNameF, inputLength + 1);

			inputLength = GetWindowTextLength(GetDlgItem(hWnd, IDTB_PASSWORD_LOGIN));
			char* passwordF = (char*)malloc(inputLength + 1);
			GetWindowText(GetDlgItem(hWnd, IDTB_PASSWORD_LOGIN), passwordF, inputLength + 1);

			/*while (fgets(line, 50, gpRegistrationDetailsFile) != NULL) {
				if (strstr(line, "User Name: ") != NULL) {
					sscanf(line, "User Name: %s", userName);
				}

				if (strstr(line, "Password: ") != NULL) {
					sscanf(line, "Password: %s", password);
				}
			}*/

			if (strcmp(gaInfo->userName, userNameF) != 0 || strcmp(gaInfo->password, passwordF) != 0) {
				MessageBox(hWnd, TEXT("Failed to login"), TEXT("Error"), MB_OK);
				bUserLoginDone = FALSE;
			}
			else {
				fprintf(gpFile, "Login successfully\n");
				bUserLoginDone = TRUE;
			}

			free(userNameF);
			free(passwordF);

			//fclose(gpRegistrationDetailsFile);
			//gpRegistrationDetailsFile = NULL;
		}
		if (bUserLoginDone == TRUE)
		{
			bRegistrationPage = FALSE;
			bLoginPage = FALSE;
			bBlackListPage = FALSE;
			bForgotPwdPage = FALSE;

			bDeviceListPage = TRUE;
			ShowHidePage(hWnd);
		}
		break;
	}
	case IDBTN_FORGOT_PASSWORD:
	{
		bRegistrationPage = FALSE;
		bLoginPage = FALSE;
		bDeviceListPage = FALSE;
		bBlackListPage = FALSE;

		bForgotPwdPage = TRUE;
		ShowHidePage(hWnd);
		break;
	}

	case IDBTN_CANCEL_FGTPWD:
	{
		bRegistrationPage = FALSE;
		bDeviceListPage = FALSE;
		bBlackListPage = FALSE;
		bForgotPwdPage = FALSE;

		bLoginPage = TRUE;
		ShowHidePage(hWnd);
		break;
	}

	case IDBTN_SEND_FGTPWD:
	{
		int inputLength = GetWindowTextLength(GetDlgItem(hWnd, IDTB_EMAILID_FGTPWD));
		char* emailID = (char*)malloc(inputLength + 1);
		GetWindowText(GetDlgItem(hWnd, IDTB_EMAILID_FGTPWD), emailID, inputLength + 1);

		if (ValidateEmail(emailID)) {
			//Send Email to Email ID which contains Password
			MessageBox(hWnd, TEXT("Send an Email to Forgot Password"), TEXT("Forgot Password"), MB_OK);

			bRegistrationPage = FALSE;
			bDeviceListPage = FALSE;
			bBlackListPage = FALSE;
			bForgotPwdPage = FALSE;

			bLoginPage = TRUE;
			ShowHidePage(hWnd);
		}
		else {
			MessageBox(hWnd, TEXT("Please enter valid email ID"), TEXT("Validation Error"), MB_OK);
		}
		break;
	}


	default:
		break;//return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
#pragma endregion

#pragma region User Related Code
BOOL CheckForRegisteredUser()
{
	gaInfo = (AdminInfo*)ReadAdminDetails();

	if (gaInfo != NULL)
	{
		MessageBox(NULL, TEXT("Admin Found"), TEXT("Validation Error"), MB_OK);
		return TRUE;
	}
	else
	{
		MessageBox(NULL, TEXT("Admin Not Found"), TEXT("Validation Error"), MB_OK);
		return FALSE;
	}
}

int ValidateEmail(char* email)
{
	int atCount = 0, dotAfterAt = 0;
	int i = 0;

	if (email == NULL || strlen(email) < 3) {
		return 0;
	}

	while (email[i] != '\0') {
		if (email[i] == '@') {
			atCount++;
			if (atCount == 1 && i > 0) {
				for (int j = i + 1; email[j] != '\0'; j++) {
					if (email[j] == '.') {
						dotAfterAt = 1;
					}
				}
			}
		}

		if (!isalnum(email[i]) && email[i] != '@' && email[i] != '.' && email[i] != '_' && email[i] != '-') {
			return 0;
		}
		i++;
	}

	return ((atCount == 1) && (dotAfterAt == 1));
}
#pragma endregion

//========== USB Related ==============
#pragma region USB Related
void RegisterUSBNotification(void)
{
	// Device Detection releated variable's
	HDEVNOTIFY hDeviceNotify = NULL;
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

	// Register for USB notificaiton
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_DISK;

	RegisterDeviceNotification(ghwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
}

void FetchUSBDeviceDetails(void)
{
	// SetupDiDestroyDeviceInfoList(hDevInfo);
	HDEVINFO dev_handle;
	DWORD member_index = 0;
	SP_DEVINFO_DATA dev_info_data;
	SP_DEVICE_INTERFACE_DATA dev_interface_data;
	
	static TCHAR str_buff[2048];

	if ((dev_handle = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE)) == INVALID_HANDLE_VALUE)
	{
		fprintf(gpFile, "Failed to get device handle\n");
		return;
	}

	dev_info_data.cbSize = sizeof(dev_info_data);
	dev_interface_data.cbSize = sizeof(dev_interface_data);

	// Enumerate devices
	for (member_index = 0;
		SetupDiEnumDeviceInfo(dev_handle, member_index, &dev_info_data);
		++member_index, dev_info_data.cbSize = sizeof(dev_info_data), dev_interface_data.cbSize = sizeof(dev_interface_data))
	{
		BOOL iteration_error = FALSE;

		// Skip devices which aren't USBSTOR
		if ((!SetupDiGetDeviceRegistryProperty(dev_handle, &dev_info_data, SPDRP_ENUMERATOR_NAME, NULL,
			(BYTE*)str_buff, sizeof(str_buff), NULL)) ||
			(_tcscmp(str_buff, TEXT("USBSTOR")) != 0))
		{
			continue;
		}

		// Buffer to hold the unique ID
		char deviceInstanceID[MAX_DEVICE_ID_LEN];

		//If we can't get instance ID (and it's not because of insufficient buffer length), silently skip to the next device
		// Retrieve the Device Instance ID (Parent Value)
		if ((!SetupDiGetDeviceInstanceId(dev_handle, &dev_info_data, deviceInstanceID, sizeof(deviceInstanceID), 0)) &&
			(GetLastError() != ERROR_INSUFFICIENT_BUFFER))
		{
			continue;
		}

		BOOL deviceLogged = FALSE;
		BOOL deviceBlackListed = FALSE;

		deviceLogged = (FindDeviceInDeviceList(deviceInstanceID) == 1) ? TRUE : FALSE;
		if (deviceLogged == TRUE)
		{
			deviceBlackListed = (FindDeviceInBlackList(deviceInstanceID) == 1) ? TRUE : FALSE;
		}

		// Try to get the device interface data
		if (!SetupDiEnumDeviceInterfaces(dev_handle, NULL, &GUID_DEVINTERFACE_DISK, member_index, &dev_interface_data))
		{
			continue;
		}
		else
		{
			PSP_DEVICE_INTERFACE_DETAIL_DATA dev_int_detail_ptr = (PSP_DEVICE_INTERFACE_DETAIL_DATA)str_buff;
			dev_int_detail_ptr->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// Try to get the device's path
			if (!SetupDiGetDeviceInterfaceDetail(dev_handle, &dev_interface_data, dev_int_detail_ptr, sizeof(str_buff), NULL, NULL))
			{
				iteration_error = TRUE;
			}
			else
			{
				// Try to get disk handle in order to get the device number
				HANDLE disk_handle = CreateFile(dev_int_detail_ptr->DevicePath,
					FILE_READ_ATTRIBUTES,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, 0, NULL);

				// Skip if we can't get the device number
				if (disk_handle == INVALID_HANDLE_VALUE)
				{
					iteration_error = TRUE;
				}
				else
				{
					DWORD bytes_returned = 0;
					BYTE disk_geometry_buff[sizeof(DISK_GEOMETRY_EX) + sizeof(DISK_PARTITION_INFO) + sizeof(DISK_DETECTION_INFO)];
					PDISK_GEOMETRY_EX storage_geometry = (PDISK_GEOMETRY_EX)disk_geometry_buff;

					// Try to get disk geometry and make sure it's a removable media (other than floppy) or external HD
					if ((!DeviceIoControl(disk_handle,
						IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0,
						(LPVOID)disk_geometry_buff, sizeof(disk_geometry_buff), &bytes_returned, NULL)) ||
						(storage_geometry->Geometry.MediaType != RemovableMedia && storage_geometry->Geometry.MediaType != FixedMedia))
					{
						iteration_error = TRUE;
					}
					else
					{
						if (deviceLogged == FALSE)
						{
							DeviceInfo* deviceInfo = (DeviceInfo*)malloc(sizeof(DeviceInfo));
							memset(deviceInfo, 0, sizeof(DeviceInfo));

							// Fetch friendly name
							if (!SetupDiGetDeviceRegistryProperty(dev_handle, &dev_info_data, SPDRP_FRIENDLYNAME, NULL,
								(BYTE*)deviceInfo->friendlyName, sizeof(deviceInfo->friendlyName), NULL)) {
								strcpy(deviceInfo->friendlyName, TEXT(""));
							}

							// Fetch manufacturer
							if (!SetupDiGetDeviceRegistryProperty(dev_handle, &dev_info_data, SPDRP_MFG, NULL,
								(BYTE*)deviceInfo->manufacturer, sizeof(deviceInfo->manufacturer), NULL)) {
								strcpy(deviceInfo->manufacturer, TEXT(""));
							}

							// Fetch device description
							if (!SetupDiGetDeviceRegistryProperty(dev_handle, &dev_info_data, SPDRP_DEVICEDESC, NULL,
								(BYTE*)deviceInfo->deviceDescription, sizeof(deviceInfo->deviceDescription), NULL)) {
								strcpy(deviceInfo->deviceDescription, TEXT(""));
							}

							// Fetch class GUID
							if (!SetupDiGetDeviceRegistryProperty(dev_handle, &dev_info_data, SPDRP_CLASSGUID, NULL,
								(BYTE*)deviceInfo->classGuid, sizeof(deviceInfo->classGuid), NULL)) {
								strcpy(deviceInfo->classGuid, TEXT(""));
							}

							// Log device info
							strcpy(deviceInfo->deviceInstanceID, deviceInstanceID);
							strcpy(deviceInfo->devicePath, dev_int_detail_ptr->DevicePath);
							sprintf(deviceInfo->totalBytes, "%lld", storage_geometry->DiskSize.QuadPart);

							fprintf(gpFile, "Parent Value (Unique ID): %s\n", deviceInfo->deviceInstanceID);
							fprintf(gpFile, "Friendly Name: %s\n", deviceInfo->friendlyName);
							fprintf(gpFile, "Manufacturer: %s\n", deviceInfo->manufacturer);
							fprintf(gpFile, "Description: %s\n", deviceInfo->deviceDescription);
							fprintf(gpFile, "Class GUID: %s\n", deviceInfo->classGuid);
							fprintf(gpFile, "Total bytes : %s\n", deviceInfo->totalBytes);
							fprintf(gpFile, "Device Path : %s\n", deviceInfo->devicePath);

							free(deviceInfo);
						}
						else
						{
							if (deviceBlackListed == TRUE)
							{
								//Try to Desconnect from here only
							}
						}
					}

					CloseHandle(disk_handle);

					if (iteration_error)
					{
						continue;
					}
				}
			}
		}

	}

	SetupDiDestroyDeviceInfoList(dev_handle);
}
#pragma endregion




#pragma region SplashScreen



void Start_Splash()
{
	WNDCLASSEX wndSplash;
	HWND hwndSplash;
	MSG msg;
	TCHAR szSplash[] = TEXT("SplashScreen");

	int screenWidth, screenHeight;
	int windowWidthHalf, windowHeightHalf;
	int windowTop, windowLeft;

	// window class Initialization
	wndSplash.cbSize = sizeof(WNDCLASSEX);
	wndSplash.lpszClassName = szSplash;
	wndSplash.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndSplash.lpfnWndProc = SplashScreenWndProc;
	wndSplash.hInstance = ghInstance;
	wndSplash.style = 0;
	wndSplash.cbClsExtra = 0;
	wndSplash.cbWndExtra = 0;
	wndSplash.hbrBackground = NULL;
	wndSplash.hIcon = NULL;
	wndSplash.lpszMenuName = NULL;
	wndSplash.hIconSm = NULL;

	// registration of window class
	RegisterClassEx(&wndSplash);

	hSplashBMP = LoadBitmap(ghInstance, MAKEINTRESOURCE(BITMAP_ID));

	if (!hSplashBMP)
	{

		MessageBox(NULL, "Failed To Load Bitmap", "Error", MB_OK | MB_ICONERROR);

		return;

	}

	BITMAP Bitmap;
	GetObject(hSplashBMP, sizeof(BITMAP), &Bitmap);

	SplashWidth = Bitmap.bmWidth;
	SplashHeight = Bitmap.bmHeight;

	screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

	windowWidthHalf = SplashWidth / 2;
	windowHeightHalf = SplashHeight / 2;

	windowLeft = (screenWidth / 2) - windowWidthHalf;
	windowTop = (screenHeight / 2) - windowHeightHalf;


	// create window
	hwndSplash = CreateWindowEx(0,
		szSplash,
		TEXT("Splash"),
		WS_POPUP,
		windowLeft,
		windowTop,
		SplashWidth,
		SplashHeight,
		NULL,
		NULL,
		ghInstance,
		NULL);

	ghwndSplash = hwndSplash;

	hSplashDC = GetDC(hwndSplash);
	hMemDC = CreateCompatibleDC(hSplashDC);
	SelectObject(hMemDC, (HGDIOBJ)hSplashBMP);

	//SetTimer(hwndSplash, 1, SPLASH_DURATION, NULL);
	// show window
	ShowWindow(hwndSplash, SW_SHOW);

	// paint background of the window
	UpdateWindow(hwndSplash);

	// Message Loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK SplashScreenWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
	{
		BitBlt((HDC)wParam, 0, 0, SplashWidth, SplashHeight, hMemDC, 0, 0, SRCCOPY);
		break;
	}
	case WM_CHAR:
	case WM_KILLFOCUS:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		DeleteObject(hSplashBMP);
		ReleaseDC(hWnd, hSplashDC);
		ReleaseDC(hWnd, hMemDC);
		DestroyWindow(hWnd);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
#pragma endregion



// ====== About Dlg Box ===========
#pragma region About Dlg Box



// Message handler for about box.
INT_PTR CALLBACK AboutWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
	{
		int iControlId = LOWORD(wParam);
		switch (iControlId)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	}
	return (INT_PTR)FALSE;
}

#pragma endregion


