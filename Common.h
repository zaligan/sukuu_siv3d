# pragma once
# include <Siv3D.hpp>

// シーンの名前
enum class State
{
	Title,
	Game
};

// シーン間で共有するデータ
struct GameData
{
	bool testMode = false;
};

using App = SceneManager<State, GameData>;
