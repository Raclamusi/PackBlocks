#pragma once


#include <windows.h>
#include <string>


class Console
{
private:
	class Initializer;

	static HANDLE handle;

public:
	static void SetCursorVisible(bool visible);
	static void SetCursorPosition(COORD coord);
	static void SetTitle(const char *title);
	static void SetTitle(const std::string &title);
	static void SetWindowSize(COORD size, bool scroll = true);
	static void SetFontSize(short size);
	static void SetTextAttribute(WORD attribute);
	static COORD GetWindowSize();
	static void Clear(SMALL_RECT region, WORD attribute = 0x07);
	static void Clear(COORD size, COORD startCoord, WORD attribute = 0x07);
	static void Clear(WORD attribute = 0x07);
};

