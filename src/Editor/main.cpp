#include <Editor/Desktop.hpp>

// Desktop desktop(384, 216, 4, TitleBarMode::Full); // 16:9
Desktop desktop(384, 240, 4, TitleBarMode::Full); // 16:10

int main(int argc, char** argv)
{
	// Debug purpose to test the correct modifications of files in argv[1]/programs
	// Otherwise it will check on the binary location
	if (argc > 1) {
		std::filesystem::current_path(std::filesystem::path(argv[1]) / "programs");
	}
	else {
		std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path() / "programs");
	}
	desktop.run();
	return 0;
}
