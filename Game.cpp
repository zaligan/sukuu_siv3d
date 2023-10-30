#include "Game.h"

Enemy::Enemy(Texture tex, Circle collider):tex(tex),collider(collider)
{

}

Enemy::~Enemy()
{
}

void Enemy::draw() const
{
	tex.drawAt(500,500);
}

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
	enemy1_coliArr << n1;
	enemy1_coliArr << n2;
	enemy1_coliArr << n3;
}

void Game::update()
{
	ClearPrint();
	Print << enemy1_coliArr;
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
		pBullet_posArr << Vec2{pJet_r + 10 ,degrees};
		pBullet_coliArr << Circle{ 0,0,pBullet_r };
		pShotAud.playOneShot();
	}
	for (auto& arr : pBullet_posArr)
	{
		arr.x += pBullet_speed * deltaTime;
	}

	for (int i = 0; i < pBullet_coliArr.size(); i++)
	{
		pBullet_coliArr.at(i).setPos(OffsetCircular({ 0,0 }, pBullet_posArr.at(i).x, pBullet_posArr.at(i).y));
		if (pBullet_posArr.at(i).x > pJet_r + 400)
		{
			pBullet_coliArr.erase(pBullet_coliArr.begin() + i);
			pBullet_posArr.erase(pBullet_posArr.begin() + i);
			i--;
			continue;
		}
		if (pBullet_coliArr.at(i).intersects(box))
		{
			pBullet_coliArr.erase(pBullet_coliArr.begin() + i);
			pBullet_posArr.erase(pBullet_posArr.begin() + i);
			i--;
			continue;
		}
		//p弾Hit処理
		for (auto it = enemy1_coliArr.begin(); it != enemy1_coliArr.end();)
		{
			if (pBullet_coliArr.at(i).intersects(*it))
			{
				pBullet_coliArr.erase(pBullet_coliArr.begin() + i);
				pBullet_posArr.erase(pBullet_posArr.begin() + i);
				i--;
				it = enemy1_coliArr.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	//カメラ計算
	radians = degrees * Math::Pi / 180;
	camera.setTargetCenter({0,-pJet_r-80});
	mat = Mat3x2::Rotate(-degrees, {0,0});
	camera.update();
	
}

void Game::draw() const
{
	Scene::SetBackground(HSV{ 230, 0.6, 0.6 });

	{
		// 2D カメラの設定から Transformer2D を作成
		const auto t0 = camera.createTransformer();
	
		const Transformer2D t1{ mat,TransformCursor::Yes };

		Circle{ 0, 0, earth_r }.draw(Palette::Brown);

		house.scaled(0.8).drawAt(0, -earth_r);
		house.scaled(0.8).rotated(Math::HalfPi).drawAt(earth_r, 0);
		house.scaled(0.8).rotated(Math::Pi).drawAt(0, earth_r);
		house.scaled(0.8).rotated(-Math::HalfPi).drawAt(-earth_r, 0);

		for (auto& bullet : pBullet_coliArr)
		{
			bullet.draw(Palette::Black);
			pBullet_tex.rotated(degrees).drawAt(bullet.center);
		}
		
		pJetTex.rotated(degrees).drawAt(pJet_pos);

		for (auto& enemy : enemy1_coliArr)
		{
			//enemy.draw(Palette::Black);
			enemy1_tex.drawAt(enemy.center);
		}
		box.draw();
		eee.draw();
	}
}
