
#include <iostream>


int main()
{
	try {
		std::cout << "PackBlocks!" << std::endl;
		std::cin.get();
	}
	catch (const std::exception &e) {
		std::system("cls");
		std::cerr << '\a' << e.what() << std::endl;
		std::cin.get();
		return 1;
	}
	return 0;
}
