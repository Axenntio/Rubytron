#include <filesystem>
#include <Runtime/Player.hpp>

int main(int argc, char** argv)
{
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());
	std::vector<std::string> params;
	for (int i  = 1; i < argc; ++i) {
		params.push_back(std::string(argv[i]));
	}
	Player player(sf::Vector2u(192, 128), 2, params);
	player.init();
	player.run();
	return 0;
}
