#include <Editor/Desktop.hpp>

Desktop desktop(384, 216, 4, TitleBarMode::Full);

int main()
{
	desktop.run();
	return 0;
}