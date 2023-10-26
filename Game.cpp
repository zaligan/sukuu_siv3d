#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
		
}


void Game::update()
{
	camera.update();
	{
		// 2D カメラの設定から Transformer2D を作成
		const auto t = camera.createTransformer();

		for (int32 i = 0; i < 8; ++i)
		{
			Circle{ 0, 0, (50 + i * 50) }.drawFrame(2);
		}

		cat.drawAt(0, 0);

		Shape2D::Star(50, Vec2{ 200, 200 }).draw(Palette::Yellow);
	}
}

void Game::draw() const
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	pJetTex.drawAt(100,100);
}
