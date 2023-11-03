#include "Game.h"

Enemy::Enemy(Vec2 r_deg) : r_deg(r_deg), old_r_deg(r_deg), currentHP(maxHP)
{
    // コンストラクタの本体は空で良い場合
}
Enemy::Enemy(double x, double y)
{
	r_deg = Vec2{ x,y };
	currentHP = maxHP;
}

Enemy::~Enemy()
{
}

void Enemy::draw() const
{
	//collider.draw(Palette::Black);
	tex.rotated(r_deg.y).drawAt(collider.center);
}

bool Enemy::calcHP(double damage)
{
	currentHP -= damage;
	if (currentHP < 0)
		return true;
	return false;
}

void Enemy::move(Vec2 to)
{
	const double t = Min(stopwatch.sF(), 1.0);
	r_deg = old_r_deg.lerp(to, t);
	Print << to;
	Print << old_r_deg;
	Print << r_deg;
	/*if (t == 1.0)
		stopwatch.restart();*/
}

void Enemy::updatePos()
{
	collider.setPos(OffsetCircular({ 0,0 }, r_deg.x, r_deg.y));
}

Circle Enemy::getCollider()
{
	return collider;
}

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{

}

void Game::update()
{
	ClearPrint();
	Print << enemy_arr.size();
	deltaTime = Scene::DeltaTime();
	pShotTimer += deltaTime;
	eShotTimer += deltaTime;

	if (KeyX.down())
	{
		enemy_arr << Enemy{ {earth_r + 150,0} };
	}
	for (auto& enemy : enemy_arr)
	{
		enemy.move({ earth_r + 180 ,1.5 });
		enemy.updatePos();
	}
	

	//p移動処理
	if (KeyA.pressed())
		radians -= pJet_speed * deltaTime;
	if (KeyD.pressed())
		radians += pJet_speed * deltaTime;
	pJet_pos = OffsetCircular({ 0,0 }, pJet_r, radians);
	Print << radians;

	//p弾発射処理
	if ((pShotCoolTime < pShotTimer) && KeySpace.pressed())
	{
		pShotTimer = fmod(pShotTimer,pShotCoolTime);
		pBullet_posArr << Vec2{pJet_r + 10 ,radians};
		pBullet_coliArr << Circle{ 0,0,pBullet_r };
		pShotAud.playOneShot();
	}
	for (auto& arr : pBullet_posArr)
	{
		arr.x += pBullet_speed * deltaTime;
	}
	//p弾移動＆消滅処理
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
		for (auto it = enemy_arr.begin(); it != enemy_arr.end();)
		{
			if (pBullet_coliArr.at(i).intersects(it -> getCollider()))
			{
				pBullet_coliArr.erase(pBullet_coliArr.begin() + i);
				pBullet_posArr.erase(pBullet_posArr.begin() + i);
				if (it -> calcHP(pBullet_damage))
				{
					it = enemy_arr.erase(it);
					eDeathAud.playOneShot();
				}
				i--;
				break;
			}
			else
			{
				++it;
			}
		}
	}
	//Enemy処理
	for (auto& enemy : enemy_arr)
	{
		if ((eShotCoolTime < eShotTimer))
		{
			eShotTimer = fmod(eShotTimer, eShotCoolTime);
			eBullet_posArr << Vec2{ enemy.getCollider().top()};
			pBullet_coliArr << Circle{ 0,0,eBullet_r };
		}
		
	}

	//カメラ計算
	camera.setTargetCenter({0,-pJet_r-80});
	mat = Mat3x2::Rotate(-radians, {0,0});
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
			pBullet_tex.rotated(radians).drawAt(bullet.center);
		}
		
		pJetTex.rotated(radians).drawAt(pJet_pos);
		for (auto& enemy : enemy_arr)
		{
			enemy.draw();
		}
		box.draw();

	}
}
