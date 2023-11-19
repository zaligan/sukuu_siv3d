# include <Siv3D.hpp> // Siv3D v0.6.12
# include"Common.h"
# include "Title.h"
# include "Game.h"

void Main()
{
	App manager;

	const TOMLReader toml{ U"config.toml" };
	if (not toml)
	{
		throw Error{ U"Failed to load 'config.toml'" };
	}
	manager.get()->earth_r = toml[U"WorldSetting.earth_r"].get<double>();
	manager.get()->houseSize = toml[U"WorldSetting.houseSize"].get<double>();
	manager.get()->enemyHouseRange = toml[U"WorldSetting.enemyHouseRange"].get<double>();

	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);

	manager.init(State::Game);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
