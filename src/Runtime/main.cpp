#include <filesystem>
#include <Runtime/Player.hpp>

std::shared_ptr<Player> player_ptr;

int main(int argc, char** argv)
{
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());
	std::vector<std::string> params;
	for (int i  = 1; i < argc; ++i) {
		params.push_back(std::string(argv[i]));
	}
	player_ptr = std::make_shared<Player>(sf::Vector2u(192, 128), 8, params);
	player_ptr->init();
	player_ptr->run();
	return 0;
}