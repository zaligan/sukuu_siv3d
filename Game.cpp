#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
	
}


void Game::update()
{
	deltaTime = Scene::DeltaTime();

	if (KeyA.pressed())
		degrees -= pJet_speed * deltaTime;
	if (KeyD.pressed())
		degrees += pJet_speed * deltaTime;
	pJet_pos = OffsetCircular({ 0,0 }, earth_r, degrees);
	radians = degrees * Math::Pi / 180;
	camera.setTargetCenter({0,-earth_r});
	Vec2 temp{ OffsetCircular({ 0,0 }, earth_r, 180) };
	mat = Mat3x2::Rotate(-degrees, {0,0});
	camera.update();
	
}

void Game::draw() const
{
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	cat.drawAt(300, 300);

	{
		// 2D カメラの設定から Transformer2D を作成
		const auto t0 = camera.createTransformer();
	
	
		const Transformer2D t1{ mat,TransformCursor::Yes };

		Circle{ 0, 0, earth_r }.drawFrame(2);

		cat.drawAt(0, 0);
		cat.drawAt(0, earth_r);
		cat.drawAt(earth_r, 0);
		
		pJetTex.rotated(degrees).drawAt(pJet_pos);
	
		
	}
}
