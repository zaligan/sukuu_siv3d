#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
		
}


void Game::update()
{

}

void Game::draw() const
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });
	houseTex.drawAt(200, 200);
	
}
