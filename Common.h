# pragma once
# include <Siv3D.hpp>

// シーンの名前
enum class State
{
	Title,
	Game
};

// 共有するデータ
struct GameData
{
	double earth_r;
	double houseSize;
	double enemyHouseRange;
	bool testMode;
};

using App = SceneManager<State, GameData>;
