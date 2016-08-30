#include "usermessage.hpp"

#ifdef WINDOWS_VERSION
#include <windows.h>

void UserMessage::msg(std::wstring str)
{
    MessageBox(NULL, (LPCWSTR)str.c_str(), (LPCWSTR)L"Message", MB_OK);
}
#else
#include "mlib.hpp"

void UserMessage::msg(std::wstring str)
{
    Out = mlib::wstr2str(str) + "\n";
}
#endif
