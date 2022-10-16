#include <Editor/Desktop.hpp>

Desktop desktop(400, 240, 8, TitleBarMode::Full);

int main()
{
	desktop.run();
	return 0;
}