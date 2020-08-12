
#include "Console.h"
#include <windows.h>
#include <iostream>
#include <cstdlib>


int main()
{
	try {
		Console::SetTitle("PackBlocks");
		Console::SetWindowSize({ 70, 24 }, false);
		Console::SetFontSize(20);
		Console::SetCursorVisible(false);

		std::cout << "PackBlocks!" << std::endl;
		std::cin.get();
	}
	catch (const std::exception &e) {
		std::system("cls");
		std::cerr << "\aƒGƒ‰[‚ª”­¶‚µ‚Ü‚µ‚½B\n\n" << e.what() << std::endl;
		std::cin.get();
		return 1;
	}
	return 0;
}
