//
#pragma once

#include <windows.h>

#include "stdio.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <String.h>

#include "../Headers/DeviceDetector.h"
#include "../Headers/DeviceDetector_JSON.h"

#define	MAX_HEIGHT	600
#define	MAX_WIDTH	800

// global function declarations
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AboutWndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
HWND ghwnd = NULL;
HMENU ghMenu = NULL;
struct RegistrationFormHandles {
	HWND hwndUserName;
	HWND hwndPassword;
} regFormHandles;

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

	// window class Initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// registration of window class
	RegisterClassEx(&wndclass);

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
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

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
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int validateEmail(char *email);

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
		break;
	}
    case WM_COMMAND:
        {
            int iControlId = LOWORD(wParam);
            // Parse the selections:
            switch (iControlId)
            {
				//Menu Commands
			case IDM_FILE_DEVICE_LIST:
			{
				break;
			}
			case IDM_FILE_BLACK_LIST:
			{
				break;
			}
			case IDM_HELP_ABOUT:
			{
				DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutWndProc);
				break;
			}
			case IDM_FILE_EXIT:
			{
				DestroyWindow(hWnd);
				break;
			}


			//Button Commands
			case IDD_BUTTON:
				bLoginPage = FALSE;
				bDeviceListPage = TRUE;
				ShowHidePage(hWnd);
				break;
            
			case IDD_LOGIN_USER:
				gpRegistrationDetailsFile = fopen("UserRegistrationDetails.txt", "r");

				if (gpRegistrationDetailsFile == NULL)
				{
					MessageBox(NULL, TEXT("Registration File Not Found."), TEXT("File I/O Error"), MB_OK);
					//exit(0);
				}
				else
				{
					int inputLength;
					char line[100];      // Buffer for reading lines from the file
					char userName[50];   // Buffer for storing the username
					char password[50]; 

					inputLength = GetWindowTextLength(regFormHandles.hwndUserName);
					char* userNameF = (char*)malloc(inputLength + 1);
					GetWindowText(regFormHandles.hwndUserName, userNameF, inputLength + 1);

					inputLength = GetWindowTextLength(regFormHandles.hwndPassword);
					char* passwordF = (char*)malloc(inputLength + 1);
					GetWindowText(regFormHandles.hwndPassword, passwordF, inputLength + 1);

					while (fgets(line, 50, gpRegistrationDetailsFile) != NULL) {
						if (strstr(line, "User Name: ") != NULL) {
						    sscanf(line, "User Name: %s", userName);
						}
				  
						if (strstr(line, "Password: ") != NULL) {
						    sscanf(line, "Password: %s", password);
						}
					}

					if(strcmp(userName, userNameF) != 0 || strcmp(password, passwordF) != 0) {
						MessageBox(NULL, TEXT("Failed to login"), TEXT("Error"), MB_OK);
					}
					else {
						fprintf(gpFile, "Login successfully\n");
					}

					free(userNameF);
					free(passwordF);

					fclose(gpRegistrationDetailsFile);
					gpRegistrationDetailsFile = NULL;
				}
				break;
		 
		     case IDD_FORGOT_PASSWORD:			
			{
				int inputLength = GetWindowTextLength(regFormHandles.hwndUserName);
				char* userNameF = (char*)malloc(inputLength + 1);
				GetWindowText(regFormHandles.hwndUserName, userNameF, inputLength + 1);

				if(validateEmail(userNameF)) {
					MessageBox(NULL, TEXT("Send an Email to Forgot Password"), TEXT("Forgot Password"), MB_OK);
				}
				else {					
					MessageBox(NULL, TEXT("Please enter valid email ID"), TEXT("Validation Error"), MB_OK);
				}
				break;
			}

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // Welcome Screen code will come here
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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

void AddMenu(HWND hWnd)
{
	ghMenu = CreateMenu();

	//File Menu
	HMENU hMenuFile = CreateMenu();
	AppendMenu(hMenuFile, MF_STRING, IDM_FILE_DEVICE_LIST, TEXT("Device List"));
	AppendMenu(hMenuFile, MF_STRING, IDM_FILE_BLACK_LIST,  TEXT("Black List"));
	AppendMenu(hMenuFile, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hMenuFile, MF_STRING, IDM_FILE_BLACK_LIST, TEXT("Exit"));

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
	::CreateWindow(
		TEXT("Static"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
		TEXT("Login"), //Name on control
		WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, //Style reqiured (Explore as required)
		170, 100, //left top position WITHIN WINDOW
		400, 40, //Size (width n height)
		hWnd, // handle of app window
		(HMENU)IDL_USER_LOGIN, // ID of button to uniquely identify in othere area of program 
		NULL,
		NULL);
	::CreateWindow(
		TEXT("Static"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
		TEXT("Username"), //Name on control
		WS_CHILD | WS_VISIBLE, //Style reqiured (Explore as required)
		200, 200, //left top position WITHIN WINDOW
		100, 25, //Size (width n height)
		hWnd, // handle of app window
		(HMENU)IDL_USERNAME, // ID of button to uniquely identify in othere area of program 
		NULL,
		NULL);
	::CreateWindow(
		TEXT("Static"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
		TEXT("Password"), //Name on control
		WS_CHILD | WS_VISIBLE, //Style reqiured (Explore as required)
		200, 250, //left top position WITHIN WINDOW
		100, 25, //Size (width n height)
		hWnd, // handle of app window
		(HMENU)IDL_PASSWORD, // ID of button to uniquely identify in othere area of program 
		NULL,
		NULL);

	regFormHandles.hwndUserName = ::CreateWindow(
		TEXT("EDIT"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
		TEXT(""), //Name on control
		WS_CHILD | WS_VISIBLE | WS_BORDER, //Style reqiured (Explore as required)
		310, 200, //left top position WITHIN WINDOW
		200, 25, //Size (width n height)
		hWnd, // handle of app window
		(HMENU)IDTB_USERNAME, // ID of button to uniquely identify in othere area of program 
		NULL,
		NULL);

	regFormHandles.hwndPassword = ::CreateWindow(
		TEXT("EDIT"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
		TEXT(""), //Name on control
		WS_CHILD | WS_VISIBLE | WS_BORDER, //Style reqiured (Explore as required)
		310, 250, //left top position WITHIN WINDOW
		200, 25, //Size (width n height)
		hWnd, // handle of app window
		(HMENU)IDTB_PASSWORD, // ID of button to uniquely identify in othere area of program 
		NULL,
		NULL);

	::CreateWindow(
		TEXT("BUTTON"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
		TEXT("Log In"), //Name on control
		WS_CHILD | WS_VISIBLE, //Style reqiured (Explore as required)
		200, 320, //left top position WITHIN WINDOW
		150, 40, //Size (width n height)
		hWnd, // handle of app window
		(HMENU)IDD_LOGIN_USER, // ID of button to uniquely identify in othere area of program 
		NULL,
		NULL);
	
	::CreateWindow(
		TEXT("BUTTON"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
		TEXT("Forgot Password"), //Name on control
		WS_CHILD | WS_VISIBLE, //Style reqiured (Explore as required)
		360, 320, //left top position WITHIN WINDOW
		150, 40, //Size (width n height)
		hWnd, // handle of app window
		(HMENU)IDD_FORGOT_PASSWORD, // ID of button to uniquely identify in othere area of program 
		NULL,
		NULL);
}

void ShowHidePage(HWND hWnd)
{
	//Registration Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bRegistrationPage);

	//LoginPage
	ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bLoginPage);

	//Forgot Password Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bForgotPwdPage);

	//Device List Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bDeviceListPage);

	//Black List Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bBlackListPage);

}

int validateEmail(char *email)
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
