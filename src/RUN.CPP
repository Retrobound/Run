/*
 ____      _             _                           _
|  _ \ ___| |_ _ __ ___ | |__   ___  _   _ _ __   __| |
| |_) / _ \ __| '__/ _ \| '_ \ / _ \| | | | '_ \ / _` |
|  _ <  __/ |_| | | (_) | |_) | (_) | |_| | | | | (_| |
|_| \_\___|\__|_|  \___/|_.__/ \___/ \__,_|_| |_|\__,_|

Retrobound 3.1: Boost your computing
(c) CodyDaFoxie 2025

Retrobound 3.1 Run

TODO: Add an icon, add menu displaying on alt key press, and make the menu itself, try to optimize this (impossible).
"I am traumatized"
- The author himself, April 13th 2025
<< run.cpp >>
*/

// Imports
#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <string.h>
#include "run.h"

// Define functions
long FAR PASCAL _export WindowProc(HWND, UINT, UINT, LONG);
long FAR PASCAL _export EditProc(HWND, UINT, UINT, LONG);
int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
void RunCommand(HWND, HWND, const char*);
BOOL BrowseDlg(HWND, LPSTR);

// Define global variables
HINSTANCE hInstance;
HWND globalWindow;
FARPROC OldEditProc;

// Browse dialog
BOOL BrowseDlg(HWND window, LPSTR fileName)
{
	 OPENFILENAME ofn;
	 fileName[0] = '\0'; 			    // Pre-initialize the variable as empty
	 memset(&ofn, 0, sizeof(ofn));	 // Memory allocation stuff
	 ofn.lStructSize = sizeof(ofn);
	 ofn.hwndOwner = window;
	 ofn.lpstrFile = fileName;
	 ofn.nMaxFile = _MAX_PATH;
	 ofn.lpstrFilter =
		  "All Executables (*.EXE, *.COM, *.BAT)\0*.EXE;*.COM;*.BAT\0"            // All executables
		  "EXE Files (*.EXE)\0*.EXE\0"                                            // .exe files only
		  "Batch Files (*.BAT)\0*.BAT\0"                                          // batch scripts only
		  "All Files (*.*)\0*.*\0\0";                                             // All files
	 ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // Path of the file must exist, the file as well

	 return GetOpenFileName(&ofn); // Return the filename
}

// Edit box process function
// (this one was the most painful function I ever wrote in my entire programming career.
long FAR PASCAL _export EditProc(HWND window, UINT msg, UINT wparam, LPARAM lparam)
{
	 switch (msg)
	 {
	 case WM_KEYDOWN: // Handle keypresses
		  switch (wparam)
		  {
		  case VK_RETURN: // If the enter key is pressed:
				RunCommand(window, globalWindow, NULL); // Run the command
				return 0;
		  case VK_ESCAPE: // If the escape key is pressed:
				PostMessage(globalWindow, WM_CLOSE, 0, 0); // Close the program
				return 0;
		  }
		  break;
	 }

	 return CallWindowProc((FARPROC)OldEditProc, window, msg, wparam, lparam); // Call window proc,
																										// I absolutely couln't get that to work
}

// Main window process
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	 // Define variables
	 static char winsmth[] = "retroboundIcon";
	 hInstance = hInst;
	 MSG message;
	 HWND window;

	 if (!hPrevInstance)
	 {
		  // Define the window class
		  WNDCLASS windowClass;
		  windowClass.style = CS_HREDRAW | CS_VREDRAW;
		  windowClass.lpfnWndProc = WindowProc;
		  windowClass.cbClsExtra = 0;
		  windowClass.cbWndExtra = 0;
		  windowClass.hInstance = hInstance;
		  windowClass.hIcon = LoadIcon(hInstance, "retroboundIcon"); // TODO: Draw an icon and change this line.
		  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		  windowClass.hbrBackground = GetStockObject(WHITE_BRUSH);
		  windowClass.lpszMenuName = NULL;
		  windowClass.lpszClassName = winsmth;

		  if (!RegisterClass(&windowClass))
		  {
				return FALSE;
		  }
	 }

	 window = CreateWindow(winsmth, "Run", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
								 CW_USEDEFAULT, CW_USEDEFAULT, 360, 188, NULL, NULL, hInstance, NULL); // Create the window
																															  // Fun fact: It is the
																															  // same size as the
																															  // Rebound11 Run window!
	 if (!window)
	 {
		  return FALSE;
	 }

	 ShowWindow(window, nCmdShow); // Show window
	 UpdateWindow(window);         // update the window

	 // Message handling loop
	 while (GetMessage(&message, NULL, 0, 0))
	 {
		  TranslateMessage(&message);
		  DispatchMessage(&message);
	 }

	 return message.wParam; // Return message.wParam
}

// The main window process function
long FAR PASCAL _export WindowProc(HWND window, UINT kmnk, UINT wparam, LONG lparam)
{
	 // define a bunch of variables

	 static char sign[] = "Enter the name of the command or a batch file"; // the displayed text
	 static char szFileName[_MAX_PATH] = ""; // this is where the filename from the browse dialogue will be stored
	 static HWND hedit, hOkButton, hCancelButton, hBrowseButton; // UI Elements
	 RECT rectangle1;
	 PAINTSTRUCT structure;
	 HDC context;
	 HFONT hFontOld;
	 HFONT hFontSmall;

	 globalWindow = window; // Set globalWindow to window. Is this a good practice? If it works, it works.

	 switch (kmnk)
	 {
	 case WM_PAINT: // TODO: try to rewrite this with WM_CREATE?
		  context = BeginPaint(window, &structure);
		  GetClientRect(window, &rectangle1);
		  rectangle1.left = 5;
		  rectangle1.top = 10;

		  // Create a smaller font cuz the bigger one WON'T FIT
		  hFontSmall = CreateFont(
				-MulDiv(10, GetDeviceCaps(context, LOGPIXELSY), 72),
				0, 0, 0,
				FW_NORMAL,
				FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE,
				"Helv");
		  hFontOld = (HFONT)SelectObject(context, hFontSmall); // Keep the old font
		  DrawText(context, sign, -1, &rectangle1,
					 DT_LEFT | DT_TOP | DT_SINGLELINE);
		  TextOut(context, 10, 65, "Open:", 5);
		  SelectObject(context, hFontOld); 							 // Go back to the old font
		  DeleteObject(hFontSmall);                            // Delete the font
		  EndPaint(window, &structure);                        // End paint
		  return 0;

	 case WM_CREATE:
		  hedit = CreateWindow(                                           // The edit box
				"EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,    // Visible, add a border, align text to left
				70, 60, 270, 30, window, (HMENU)IDC_EDIT, ((LPCREATESTRUCT)lparam)->hInstance, NULL);

		  hOkButton = CreateWindow(
				"BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 120, 100, 25, window, 	  // The okay button
				(HMENU)IDC_OKBUTTON, ((LPCREATESTRUCT)lparam)->hInstance, NULL);                 	  //
																																  // ----------------
		  hCancelButton = CreateWindow(                                                             //
				"BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 120, 120, 100, 25, window, // The cancel button
				(HMENU)IDC_CANCELBUTTON, ((LPCREATESTRUCT)lparam)->hInstance, NULL);                  //
																																  // ----------------
		  hBrowseButton = CreateWindow(                                                             //
				"BUTTON", "Browse", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 230, 120, 100, 25, window, // The browse button
				(HMENU)IDC_BROWSEBUTTON, ((LPCREATESTRUCT)lparam)->hInstance, NULL);

		  // My most hated two lines of code
		  OldEditProc = (FARPROC)GetWindowLong(hedit, GWL_WNDPROC);
		  SetWindowLong(hedit, GWL_WNDPROC, (LONG)(FARPROC)EditProc);
		  // TIL I hate subclassing
		  break;

	 // Handle keypresses in the main window
	 case WM_KEYDOWN:
		  switch (wparam)
		  {
		  // I PLAYED THESE GAMES BEFORE.
		  case VK_RETURN:
				RunCommand(hedit, window, NULL);
				break;
		  case VK_ESCAPE:
				PostMessage(window, WM_CLOSE, 0, 0);
				break;
		  }
		  return 0;
	 // Handle commands
	 case WM_COMMAND:
		  switch (LOWORD(wparam))
		  {
		  case IDC_OKBUTTON: 									  // OK Button: Run the program
				RunCommand(hedit, window, NULL);
				break;
		  case IDC_CANCELBUTTON:                     	  // Cancel button: Exit!
				PostMessage(window, WM_CLOSE, 0, 0);
				break;
		  case IDC_BROWSEBUTTON:                     	  // Browse button: Open the browse file prompt
				if (BrowseDlg(window, szFileName))     	  // Call the BrowseDlg function which is at the top of the file.
				{
					 RunCommand(hedit, window, szFileName);  // Run the command
				}
				break;
		  }
		  return 0;
	 // Is this needed?
	 case WM_DESTROY:
		  PostQuitMessage(0);
		  return 0;
	 default:
		  return DefWindowProc(window, kmnk, wparam, lparam); // Return DefWindowProc
	 }
}

// Run a command and handle it
void RunCommand(HWND text, HWND window, const char* filePath)
{
	 char buffer[_MAX_PATH];

	 // If FilePath is not empty, that means the Browse button was used
	 if (filePath != NULL && strlen(filePath) > 0)
	 {
		strcpy(buffer, filePath); // Copy the filePath into the buffer
	 }
	 else {
		// If it is empty, the text box have been used
		GetWindowText(text, buffer, sizeof(buffer));
	 }
	 // Is buffer empty?
	 if (strlen(buffer) > 0)
	 {
		  ShowWindow(window, SW_HIDE);                 		// Hide the window for program execution
		  UINT result = WinExec(buffer, SW_SHOW);    		// Execute the program
		  if (result > 31) {                               // If result is greater than 31, the program executed correctly!
					PostMessage(window, WM_CLOSE, 0, 0);		// End the execution of RUN

					// Though, run.exe is still being executed in the background until the ran program finishes its execution.
					// TODO: Is there a way to handle it a different way?
		  }
		  else {                              					// Handle the errors
				switch (result) {
					case 2:  // ERROR_FILE_NOT_FOUND
						MessageBox(NULL, "The selected file has not been found. Make sure the destination file exists.", "Error", MB_OK | MB_ICONHAND);
						break;
					case 3:  // ERROR_PATH_NOT_FOUND
						MessageBox(NULL, "The path of the selected file has not been found. Make sure the destination path exists.", "Error", MB_OK | MB_ICONHAND);
						break;
					case 11: // ERROR_BAD_FORMAT
						MessageBox(NULL, "The selected file is not a valid Windows application", "Error", MB_OK | MB_ICONHAND);
						break;
					default: // Different error type
						MessageBox(NULL, "An unknown error occured.", "Error", MB_OK | MB_ICONHAND);
						break;
					}
				ShowWindow(window, SW_SHOW);  // If an error happened, show the window again.
	 }
	 }
	 else {  // If the buffer is empty, that means the user did not enter anything. Just beep then :3
		MessageBeep(0);
	 }
}
