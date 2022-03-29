#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <algorithm>

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	const int W = 1920;
	const int H = 1080;

	sf::RenderWindow window(sf::VideoMode(W, H), "Cellular Automata", sf::Style::Default, settings);

	enum class TileType: uint8_t
	{
		AIR,
		WALL,
		WATER,
	};

	struct TileInfo
	{
		TileType type;
	};

	const int TILE_SIZE = 6;
	const int MAP_W = W / TILE_SIZE;
	const int MAP_H = H / TILE_SIZE;

	TileInfo map[MAP_W][MAP_H]{ TileType::AIR };
	TileInfo map2[MAP_W][MAP_H]{ TileType::AIR };


	sf::RectangleShape wall(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	wall.setFillColor(sf::Color(128, 128, 86));

	sf::RectangleShape water(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	water.setFillColor(sf::Color(32, 32, 156));

	TileType draw_mode = TileType::WALL;

	sf::Clock clock;
	sf::Time last_update = clock.getElapsedTime();

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist2(0, 1);

	static int TPS = 1024;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		{
			draw_mode = TileType::WALL;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		{
			draw_mode = TileType::WATER;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		{
			draw_mode = TileType::AIR;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			auto mouse_pos = sf::Mouse::getPosition(window);

			if (mouse_pos.x >= 0 && mouse_pos.x < W &&
				mouse_pos.y >= 0 && mouse_pos.y < H)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
				{
					for (int x = (mouse_pos.x / TILE_SIZE) - 4; x < (mouse_pos.x / TILE_SIZE) + 4; x++)
					{
						for (int y = (mouse_pos.y / TILE_SIZE) - 4; y < (mouse_pos.y / TILE_SIZE) + 4; y++)
						{
							if (x >= 0 && x < MAP_W &&
								y >= 0 && y < MAP_H)
							{
								map[x][y].type = draw_mode;
							}
						}
					}
				}
				else
				{
					int x = mouse_pos.x / TILE_SIZE;
					int y = mouse_pos.y / TILE_SIZE;

					//std::cout << x << " " << y << std::endl;
					//std::cout << "xxxxxx " << mouse_pos.x / float(TILE_SIZE) << " " << mouse_pos.y / float(TILE_SIZE) << std::endl;

					map[x][y].type = draw_mode;
				}
			}
		}

		auto time = clock.getElapsedTime();
		int ticks = (time - last_update).asSeconds() * TPS;

		for (int i = 0; i < ticks; ++i)
		{
			for (int a = 0; a < MAP_H; ++a)
			{
				for (int b = 0; b < MAP_W; ++b)
				{
					map2[b][a] = map[b][a];
				}
			}

			//for (int y = MAP_H - 1; y >= 0; --y)
			for (int y = 0; y < MAP_H; ++y)
			{
				for (int x = 0; x < MAP_W; ++x)
				{
					if (map2[x][y].type == TileType::WATER)
					{
						if (y == MAP_H - 1)
						{
							map[x][y].type = TileType::AIR;
						}
						else
						{
							int dir = (dist2(rng) == 0 ? 1 : -1);

							if (map2[x][y + 1].type == TileType::AIR)
							{
								std::swap(map[x][y + 1], map[x][y]);
							}
							else if (x + dir >= 0 && x + dir < MAP_W && map2[x + dir][y + 1].type == TileType::AIR)
							{
								std::swap(map[x + dir][y + 1], map[x][y]);
							}
							else if (x - dir >= 0 && x - dir < MAP_W && map2[x - dir][y + 1].type == TileType::AIR)
							{
								std::swap(map[x - dir][y + 1], map[x][y]);
							}
							else if (x + dir >= 0 && x + dir && map2[x + dir][y].type == TileType::AIR)
							{
								std::swap(map[x + dir][y], map[x][y]);
							}
							else if (x - dir >= 0 && x - dir && map2[x - dir][y].type == TileType::AIR)
							{
								std::swap(map[x - dir][y], map[x][y]);
							}
						}
					}
				}
			}
		}

		if (ticks > 0)
		{
			last_update = time;
		}

		window.clear(sf::Color::Black);

		for (int i = 0; i < MAP_W; ++i)
		{
			for (int j = 0; j < MAP_H; ++j)
			{
				float x = i * TILE_SIZE;
				float y = j * TILE_SIZE;
				
				switch (map[i][j].type)
				{
					case TileType::WALL:
						wall.setPosition(x, y);
						window.draw(wall);
						break;
					case TileType::WATER:
						water.setPosition(x, y);
						window.draw(water);
						break;
					default:
						break;
				}
			}
		}

		window.display();
	}
	return 0;
}