#include <Editor/Desktop.hpp>

// Desktop desktop(384, 216, 4, TitleBarMode::Full); // 16:9
Desktop desktop(384, 240, 4, TitleBarMode::Full); // 16:10

int main()
{
	desktop.run();
	return 0;
}