//
#pragma once

#include <windows.h>


#include "stdio.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "cjson/cJSON.h"
#include "DeviceDetector.h"
#include "Headers/DeviceDetector_JSON.h"


#define	MAX_HEIGHT	600
#define	MAX_WIDTH	800

// global function declarations
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AboutWndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
HWND ghwnd = NULL;
HMENU ghMenu = NULL;

// variable related to File I/O
char gszLogFileName[] = "Log.txt";
FILE* gpFile = NULL;

//Local functions declarations
void showFileDeviceList(HWND,int);
int showDeviceListPageNumber = 1;
int totalNoOfDevicesToShow = 50;

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
				bDeviceListPage = TRUE;
				bLoginPage = FALSE;

				HWND hLoginButton = GetDlgItem(hWnd, IDD_BUTTON);
				if (hLoginButton)
				{
					ShowWindow(hLoginButton, SW_HIDE);
				}

				::CreateWindow(
					TEXT("Static"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
					TEXT("Attached Devices"), //Name on control
					WS_CHILD | WS_VISIBLE | SS_CENTER, //Style reqiured (Explore as required)
					350, 20, //left top position WITHIN WINDOW
					150, 20, //Size (width n height)
					hWnd, // handle of app window
					(HMENU)IDD_BUTTON, // ID of button to uniquely identify in othere area of program 
					NULL,
					NULL);

				showFileDeviceList(hWnd,showDeviceListPageNumber);
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

				//next button on show device list page
			case IDD_NEXT_BUTTON_SDL:
				showDeviceListPageNumber++;
				showFileDeviceList(hWnd, showDeviceListPageNumber);
				break;

				//previous button on show device list page
			case IDD_PREV_BUTTON_SDL:
				showDeviceListPageNumber--;
				showFileDeviceList(hWnd, showDeviceListPageNumber);
				break;

			case IDD_FIRST_PAGE_BUTTON_SDL:
				showDeviceListPageNumber = 1;
				showFileDeviceList(hWnd, showDeviceListPageNumber);
				break;
			case IDD_END_PAGE_BUTTON_SDL:
				showDeviceListPageNumber = totalNoOfDevicesToShow;
				showFileDeviceList(hWnd, showDeviceListPageNumber);
				break;
            
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
		TEXT("Button"), //Button type (Mostly Needed 1. Button 2. Static, 3. Edit)
		TEXT("Login"), //Name on control
		WS_CHILD | WS_VISIBLE , //Style reqiured (Explore as required)
		100, 100, //left top position WITHIN WINDOW
		50, 50, //Size (width n height)
		hWnd, // handle of app window
		(HMENU)IDD_BUTTON, // ID of button to uniquely identify in othere area of program 
		NULL, 
		NULL);


	
	
}

void ShowHidePage(HWND hWnd)
{
	//Registration Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bRegistrationPage);

	//LoginPage
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bLoginPage);

	//Forgot Password Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bForgotPwdPage);

	//Device List Page
	ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bDeviceListPage);

	//Black List Page
	//ShowWindow(GetDlgItem(hWnd, IDD_BUTTON), bBlackListPage);

}

void CreateDeviceListDialogControls(HWND hWnd, char *category, char *name, char *config, char *device_description, char *manufacturer,char *class_guid)
{
	
	::CreateWindow(
		TEXT("Static"), 
		TEXT("Category :"), 
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		150, 70, 
		150, 20, 
		hWnd, 
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"),
		TEXT(category), 
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		400, 70, 
		200, 20, 
		hWnd, 
		(HMENU)IDD_BUTTON, 
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"), 
		TEXT("Device Name :"), 
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		150, 120, 
		150, 20, 
		hWnd, 
		(HMENU)IDD_BUTTON, 
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"), 
		TEXT(name), 
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		400, 120,
		200, 20,
		hWnd, 
		(HMENU)IDD_BUTTON,  
		NULL,
		NULL);


	::CreateWindow(
		TEXT("Static"),
		TEXT("Configuration : "),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		150, 170, 
		150, 20, 
		hWnd, 
		(HMENU)IDD_BUTTON, 
		NULL,
		NULL);

	
	::CreateWindow(
		TEXT("Static"), 
		TEXT(config), 
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		400, 170, 
		200, 20, 
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"),
		TEXT("Manufacturer : "),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		150, 220,
		150, 20,
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"),
		TEXT(manufacturer),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		400, 220,
		200, 20,
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"),
		TEXT("Device Description : "),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		150, 270,
		150, 20,
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"),
		TEXT(device_description),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		400, 270,
		220, 20,
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"),
		TEXT("Class GUID : "),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		150, 320,
		150, 20,
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"),
		TEXT(class_guid),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		400, 320,
		220, 20,
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Static"),
		TEXT("Comments :"),
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		150, 370,
		150, 20,
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	::CreateWindow(
		TEXT("Edit"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE,
		400, 370,
		250, 50,
		hWnd,
		(HMENU)IDD_BUTTON,
		NULL,
		NULL);

	//show First page button
	::CreateWindow(
		TEXT("Button"),
		TEXT("|<"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
		270, 450,
		30, 30,
		hWnd,
		(HMENU)IDD_FIRST_PAGE_BUTTON_SDL,
		NULL,
		NULL);

	//Prev button on show device list page
	::CreateWindow(
		TEXT("Button"),
		TEXT("<"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,  
		300, 450, 
		30, 30,
		hWnd, 
		(HMENU)IDD_PREV_BUTTON_SDL,
		NULL,
		NULL);

	//Page Number 
	//convert page number into string
	//TEXT(numberText);
	//wsprintf(numberText, L"%d/%d", showDeviceListPageNumber , totalNoOfDevicesToShow);
	//wchar_t numberText[30];
	//wsprintf(numberText, L"%d/%d", showDeviceListPageNumber, totalNoOfDevicesToShow);
	char numberText[30];
	
	sprintf(numberText, "%d/%d", showDeviceListPageNumber, totalNoOfDevicesToShow);

	::CreateWindow(
		TEXT("Static"),
		TEXT(numberText),
		WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
		400, 450, 
		40, 25, 
		hWnd, 
		(HMENU)IDD_TEXT, 
		NULL,
		NULL);


	//Next Button on show device list page
	::CreateWindow(
		TEXT("Button"),
		TEXT(">"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
		500, 450, 
		30, 30, 
		hWnd, 
		(HMENU)IDD_NEXT_BUTTON_SDL,
		NULL,
		NULL);

	//show last page button
	::CreateWindow(
		TEXT("Button"),
		TEXT(">|"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
		530, 450,
		30, 30,
		hWnd,
		(HMENU)IDD_END_PAGE_BUTTON_SDL,
		NULL,
		NULL);

	::CreateWindow( 
		TEXT("Button"), 
		TEXT("Delete"), 
		WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
		280, 485, 
		60, 30,
		hWnd, 
		(HMENU)IDD_DELETE_BUTTON_SDL, 
		NULL,
		NULL); 

	::CreateWindow(
		TEXT("Button"),
		TEXT("Add to Black List"),
		WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
		460, 485,
		120, 30,
		hWnd,
		(HMENU)IDD_ADD_TO_BLACK_LIST_BUTTON_SDL,
		NULL,
		NULL);
}

void showFileDeviceList(HWND hwnd,int pageNo)
{
	HDC hdc;
	RECT rc;
	char category[50], name[50], config[100];
	char device_description[50], manufacturer[50], class_guid[50];

	bDeviceListPage = TRUE; 
	bLoginPage = FALSE; 

	HWND hLoginButton = GetDlgItem(hwnd, IDD_BUTTON);
	if (hLoginButton)
	{
		ShowWindow(hLoginButton, SW_HIDE);
	}

	GetClientRect(hwnd, &rc);
	hdc = GetDC(hwnd);


	//for (int i = 0; i <= 5; i++)
	//{

	if (pageNo > totalNoOfDevicesToShow)
	{
		pageNo = 1;
	}
		char * deviceInfo = getDeviceByID(pageNo);
	
		parseDeviceString(deviceInfo, category, (unsigned int)sizeof(category) , name, (unsigned int)sizeof(name), config, (unsigned int)sizeof(config));
		
		fprintf(gpFile,"Category: %s\n", category);
		fprintf(gpFile,"Name:%s\n", name);
		fprintf(gpFile, "config:%s\n\n\n", config);


		CreateDeviceListDialogControls(hwnd, category, name, config,device_description, manufacturer,class_guid);

	//}
}



