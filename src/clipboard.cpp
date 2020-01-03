#include <string>
#include <windows.h>
void get_clipboard(std::string &content)
{
	OpenClipboard(NULL);
	HANDLE handle = GetClipboardData(CF_TEXT);
	char* pszText = static_cast<char*>(GlobalLock(handle));
	content = pszText;
	GlobalUnlock(handle);
	CloseClipboard();
}
void set_clipboard(std::string &content)
{
	const size_t len = strlen(content.c_str()) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), content.c_str(), len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}