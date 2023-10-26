#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
		
}


void Game::update()
{
	pJet_pos = OffsetCircular();
	if (KeyA.pressed())
		pJet_pos.x -= 10.0;
	
	camera.update();
	camera.setTargetCenter(pJet_pos);
}

void Game::draw() const
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	cat.drawAt(300, 300);

	{
		// 2D カメラの設定から Transformer2D を作成
		const auto t = camera.createTransformer();

		Circle{ 0, 0, earth_r }.drawFrame(2);

		cat.drawAt(0, 0);
		pJetTex.drawAt(pJet_pos);
		
	}
}
