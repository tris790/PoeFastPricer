#include <string>
#include <windows.h>
#include <iostream>

void fake_ctrl_c()
{
	int key_count = 4;

	INPUT* input = new INPUT[key_count];
	for (int i = 0; i < key_count; i++)
	{
		input[i].ki.dwFlags = 0;
		input[i].type = INPUT_KEYBOARD;
	}

	input[0].ki.wVk = VK_CONTROL;
	input[0].ki.wScan = MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC);
	input[1].ki.wVk = 0x43; // Virtual key code for 'c'
	input[1].ki.wScan = MapVirtualKey(0x43, MAPVK_VK_TO_VSC);
	input[2].ki.dwFlags = KEYEVENTF_KEYUP;
	input[2].ki.wVk = input[0].ki.wVk;
	input[2].ki.wScan = input[0].ki.wScan;

	input[3].ki.dwFlags = KEYEVENTF_KEYUP;
	input[3].ki.wVk = input[1].ki.wVk;
	input[3].ki.wScan = input[1].ki.wScan;

	SendInput(key_count, (LPINPUT)input, sizeof(INPUT));
	Sleep(100);
	delete[] input;
}

void get_clipboard(std::string &content)
{
	OpenClipboard(NULL);
	HANDLE handle = GetClipboardData(CF_TEXT);
	if(handle) 
	{
		char* pszText = static_cast<char*>(GlobalLock(handle));
		if (pszText)
		{
			content = pszText;
			GlobalUnlock(handle);
			CloseClipboard();
		}
	}
	else
	{
		std::cout << "Clipboard in use" << std::endl;
	}
}
void set_clipboard(std::string &content)
{
	const size_t len = strlen(content.c_str()) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	if (hMem)
	{
		LPVOID hMemLock = GlobalLock(hMem);
		if (hMemLock) 
		{
			memcpy(hMemLock, content.c_str(), len);
			GlobalUnlock(hMem);
			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hMem);
			CloseClipboard();
		}
	}
}