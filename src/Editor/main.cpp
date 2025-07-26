#include <Editor/Desktop.hpp>

// Desktop desktop(384, 216, 4, TitleBarMode::Full); // 16:9
Desktop desktop(384, 240, 4, TitleBarMode::Full); // 16:10

int main(int argc [[maybe_unused]], char** argv)
{
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path() / "programs");
	desktop.run();
	return 0;
}
