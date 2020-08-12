
#define FAILED_CHECK(func, ...) \
do { \
	if (!func(__VA_ARGS__)) { \
		DWORD code = GetLastError(); \
		char message[256] = ""; \
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, message, 255, NULL); \
		throw std::runtime_error( \
			"Win32エラー: " #func " に失敗しました。\n  エラーコード: " + \
			std::to_string(code) + ": " + message); \
	} \
} while (false)


#include "Console.h"
#include <windows.h>
#include <stdexcept>
#include <vector>


HANDLE Console::handle;


class Console::Initializer
{
public:
	Initializer()
	{
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
			throw std::runtime_error(
				std::string("GetStdHandle に失敗しました。(エラーコード: ") +
				std::to_string(GetLastError()) + ")");
		}
	}
} initializer{};


void Console::SetCursorVisible(bool visible)
{
	CONSOLE_CURSOR_INFO cci;
	FAILED_CHECK(GetConsoleCursorInfo, handle, &cci);
	cci.bVisible = visible;
	FAILED_CHECK(SetConsoleCursorInfo, handle, &cci);
}

void Console::SetCursorPosition(COORD coord)
{
	FAILED_CHECK(SetConsoleCursorPosition, handle, coord);
}

void Console::SetTitle(const char *title)
{
	FAILED_CHECK(SetConsoleTitle, TEXT(title));
}

void Console::SetTitle(const std::string &title)
{
	SetTitle(title.c_str());
}

void Console::SetWindowSize(COORD size, bool scroll)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	FAILED_CHECK(GetConsoleScreenBufferInfo, handle, &csbi);
	SMALL_RECT srct = csbi.srWindow;
	srct.Right = srct.Left + size.X - 1;
	srct.Bottom = srct.Top + size.Y - 1;
	COORD crd = { size.X, scroll ? 300 : size.Y };
	if (srct.Right - srct.Left + 1 > size.X) {
		FAILED_CHECK(SetConsoleWindowInfo, handle, TRUE, &srct);
		FAILED_CHECK(SetConsoleScreenBufferSize, handle, crd);
	}
	else {
		FAILED_CHECK(SetConsoleWindowInfo, handle, TRUE, &srct);
		FAILED_CHECK(SetConsoleScreenBufferSize, handle, crd);
	}
}

void Console::SetFontSize(short size)
{
	CONSOLE_FONT_INFOEX cfix = { sizeof(cfix) };
	FAILED_CHECK(GetCurrentConsoleFontEx, handle, FALSE, &cfix);
	cfix.dwFontSize.X = 1;
	cfix.dwFontSize.Y = size;
	FAILED_CHECK(SetCurrentConsoleFontEx, handle, FALSE, &cfix);
}

void Console::SetTextAttribute(WORD attribute)
{
	FAILED_CHECK(SetConsoleTextAttribute, handle, attribute);
}

COORD Console::GetWindowSize()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	FAILED_CHECK(GetConsoleScreenBufferInfo, handle, &csbi);
	SMALL_RECT srct = csbi.srWindow;
	return { srct.Right - srct.Left + 1, srct.Bottom - srct.Top + 1 };
}

void Console::Clear(SMALL_RECT region, WORD attribute)
{
	COORD size = { region.Right - region.Left + 1, region.Bottom - region.Top + 1 };
	std::vector<CHAR_INFO> buffer(size.X * size.Y, { '\0', attribute });
	FAILED_CHECK(WriteConsoleOutput, handle, buffer.data(), size, { 0, 0 }, &region);
}

void Console::Clear(COORD size, COORD startCoord, WORD attribute)
{
	SMALL_RECT region = { startCoord.X, startCoord.Y, startCoord.X + size.X - 1, startCoord.Y + size.Y - 1 };
	std::vector<CHAR_INFO> buffer(size.X * size.Y, { '\0', attribute });
	FAILED_CHECK(WriteConsoleOutput, handle, buffer.data(), size, { 0, 0 }, &region);
}

void Console::Clear(WORD attribute)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	FAILED_CHECK(GetConsoleScreenBufferInfo, handle, &csbi);
	Clear(csbi.dwSize, { 0, 0 }, attribute);
}
