#include <Editor/Desktop.hpp>

Desktop desktop(192, 128, 8, TitleBarMode::Full);

int main()
{
	desktop.run();
	return 0;
}