#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
	
}


void Game::update()
{
	deltaTime = Scene::DeltaTime();
	pShotTimer += deltaTime;

	//p移動処理
	if (KeyA.pressed())
		degrees -= pJet_speed * deltaTime;
	if (KeyD.pressed())
		degrees += pJet_speed * deltaTime;
	pJet_pos = OffsetCircular({ 0,0 }, pJet_r, degrees);

	//p弾発射処理
	if ((pShotCoolTime < pShotTimer) && KeySpace.pressed())
	{
		pShotTimer = fmod(pShotTimer,pShotCoolTime);
		pBullet_deg_r << Vec2{ degrees,pJet_r};
	}
	for (auto& bullet : pBullet_arr)
	{
		bullet.y += deltaTime * pBullet_speed;
	}
	pBullet_deg_r.remove_if([r = earth_r](const Vec2& b) { return (b.y > (r + 400)); });
	for (auto& arr : pBullet_arr)
	{
		
	}
	//p弾,敵機判定
	pBullet_arr.remove_if([p = box](const Circle& b) {return b.intersects(p); });
	for (auto& bullet : pBullet_arr)
	{
		if (bullet.intersects(box))
			Print << U"HIT";
	}
	//カメラ計算
	radians = degrees * Math::Pi / 180;
	camera.setTargetCenter({0,-pJet_r-80});
	mat = Mat3x2::Rotate(-degrees, {0,0});
	camera.update();
	
}

void Game::draw() const
{
	Scene::SetBackground(HSV{ 220, 0.4, 1.0 });

	{
		// 2D カメラの設定から Transformer2D を作成
		const auto t0 = camera.createTransformer();
	
	
		const Transformer2D t1{ mat,TransformCursor::Yes };

		Circle{ 0, 0, earth_r }.draw(Palette::Brown);

		house.drawAt(0, -earth_r);
		house.rotated(Math::HalfPi).drawAt(earth_r, 0);
		house.rotated(Math::Pi).drawAt(0, earth_r);
		house.rotated(-Math::HalfPi).drawAt(-earth_r, 0);

		for (auto& bullet : pBullet_arr)
		{
			pBullet_tex.rotated(degrees).drawAt(OffsetCircular({0,0}, bullet.y, bullet.x));
			bullet.draw(Palette::Black);
		}
		
		pJetTex.rotated(degrees).drawAt(pJet_pos);
		
		box.draw();
	}
}
