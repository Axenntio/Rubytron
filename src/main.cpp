#include <iostream>
#include <Desktop.hpp>

int main(int argc, char** argv) {
	Desktop desktop(192, 128, 6);
	desktop.run();
	std::cout << "Initial work on Rubytron" << std::endl;
	return 0;
}