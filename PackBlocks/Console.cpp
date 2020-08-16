
#include "Console.h"
#include <windows.h>
#include <stdexcept>
#include <vector>


#define CHECK_FAILURE(func, ...) \
do { \
	if (FAILED((func)(__VA_ARGS__))) { \
		DWORD code = GetLastError(); \
		char message[256] = ""; \
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, message, 255, NULL); \
		throw std::runtime_error( \
			"Win32エラー: " #func " に失敗しました。\n  エラーコード: " + \
			std::to_string(code) + ": " + message); \
	} \
} while (false)

#define CHECK_HANDLE(handle, func, ...) \
do { \
	(handle) = (func)(__VA_ARGS__); \
	if ((handle) == INVALID_HANDLE_VALUE) { \
		DWORD code = GetLastError(); \
		char message[256] = ""; \
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, message, 255, NULL); \
		throw std::runtime_error( \
			"Win32エラー: " #func " に失敗しました。\n  エラーコード: " + \
			std::to_string(code) + ": " + message); \
	} \
} while (false)


HANDLE Console::handle = GetStdHandle(STD_OUTPUT_HANDLE);


void Console::SetCursorVisible(bool visible)
{
	CONSOLE_CURSOR_INFO cci;
	CHECK_FAILURE(GetConsoleCursorInfo, handle, &cci);
	cci.bVisible = visible;
	CHECK_FAILURE(SetConsoleCursorInfo, handle, &cci);
}

void Console::SetCursorPosition(COORD coord)
{
	CHECK_FAILURE(SetConsoleCursorPosition, handle, coord);
}

void Console::SetTitle(const char *title)
{
	CHECK_FAILURE(SetConsoleTitle, TEXT(title));
}

void Console::SetTitle(const std::string &title)
{
	SetTitle(title.c_str());
}

void Console::SetWindowSize(COORD size, bool scroll)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CHECK_FAILURE(GetConsoleScreenBufferInfo, handle, &csbi);
	SMALL_RECT srct1 = { 0, 0, size.X - 1, size.Y - 1 };
	COORD crd = { size.X, scroll ? 300 : size.Y };
	if (size.X > csbi.dwSize.X) size.X = csbi.dwSize.X;
	if (size.Y > csbi.dwSize.Y) size.Y = csbi.dwSize.Y;
	SMALL_RECT srct2 = { 0, 0, size.X - 1, size.Y - 1 };
	CHECK_FAILURE(SetConsoleWindowInfo, handle, TRUE, &srct2);
	CHECK_FAILURE(SetConsoleScreenBufferSize, handle, crd);
	if (srct1.Right != srct2.Right || srct1.Bottom != srct2.Bottom) {
		CHECK_FAILURE(SetConsoleWindowInfo, handle, TRUE, &srct1);
	}
}

void Console::SetFontSize(short size)
{
	CONSOLE_FONT_INFOEX cfix = { sizeof(cfix) };
	CHECK_FAILURE(GetCurrentConsoleFontEx, handle, FALSE, &cfix);
	cfix.dwFontSize.X = 1;
	cfix.dwFontSize.Y = size;
	CHECK_FAILURE(SetCurrentConsoleFontEx, handle, FALSE, &cfix);
}

void Console::SetTextAttribute(WORD attribute)
{
	CHECK_FAILURE(SetConsoleTextAttribute, handle, attribute);
}

COORD Console::GetWindowSize()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CHECK_FAILURE(GetConsoleScreenBufferInfo, handle, &csbi);
	SMALL_RECT srct = csbi.srWindow;
	return { srct.Right - srct.Left + 1, srct.Bottom - srct.Top + 1 };
}

void Console::Clear(SMALL_RECT region, WORD attribute)
{
	COORD size = { region.Right - region.Left + 1, region.Bottom - region.Top + 1 };
	std::vector<CHAR_INFO> buffer(size.X * size.Y, { '\0', attribute });
	CHECK_FAILURE(WriteConsoleOutput, handle, buffer.data(), size, { 0, 0 }, &region);
}

void Console::Clear(COORD size, COORD startCoord, WORD attribute)
{
	SMALL_RECT region = { startCoord.X, startCoord.Y, startCoord.X + size.X - 1, startCoord.Y + size.Y - 1 };
	std::vector<CHAR_INFO> buffer(size.X * size.Y, { '\0', attribute });
	CHECK_FAILURE(WriteConsoleOutput, handle, buffer.data(), size, { 0, 0 }, &region);
}

void Console::Clear(WORD attribute)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CHECK_FAILURE(GetConsoleScreenBufferInfo, handle, &csbi);
	Clear(csbi.dwSize, { 0, 0 }, attribute);
}
