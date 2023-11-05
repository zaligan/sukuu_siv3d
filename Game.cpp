#include "Game.h"



Enemy::Enemy(Vec2 from,Vec2 to) : from(from),to(to), currentHP(maxHP)
{
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
	if (currentHP <= 0)
		return true;
	else
		return false;
}

void Enemy::move()
{
	const double t = Min(stopwatch.sF() / 10, 1.0);
	r_deg = from.lerp(to, t);
	collider.setPos(OffsetCircular({ 0,0 }, r_deg.x,r_deg.y));
}

Circle Enemy::getCollider()
{
	return collider;
}

Vec2 Enemy::getFrom()
{
	return from;
}

Vec2 Enemy::getTo()
{
	return to;
}





// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
	townArr << Town{town0,townHP} << Town{ town1,townHP } << Town{ town2,townHP } << Town{ town3,townHP };
	pJet_HP = pJet_MaxHP;
}

bool Game::calcPJetHP(Circle bullet, Circle pJet)
{
	if (bullet.intersects(pJet))
	{
		pJet_HP -= eBullet_damage;
		return true;
	}
	return false;
}



void Game::update()
{
	gameBGM.play();

	//Print表示
	ClearPrint();
	for (int i = 0; i < townArr.size(); i++)
	{
		Print << i << U" TownHP:" << townArr.at(i).townHP;
	}
	if (KeyP.pressed())
		return;

	//時間管理
	deltaTime = Scene::DeltaTime();
	pShotTimer += deltaTime;
	eSpawnTimer += deltaTime;

	if (pJet_HP <= 0)
		gameOverFlag = true;
	//ゲームオーバー
	if (gameOverFlag)
	{
		if (KeyJ.down())
			changeScene(State::Title);
		return;
	}

	if (enemy_arr.size() < 50 && eSpawnCoolTime < eSpawnTimer)
	{
		eSpawnTimer -= eSpawnCoolTime;
		Vec2 fromRandom{Random<double>(earth_r + 150,earth_r + 200),Random<double>(0,2*Math::Pi)};
		Vec2 toRandom{ Random<double>(earth_r + 150,earth_r + 200),Random<double>(fromRandom.y-Math::HalfPi,fromRandom.y + Math::HalfPi)};
		enemy_arr << Enemy{ fromRandom,toRandom };
	}

	shieldFlag = KeyK.pressed();
	
	//p移動処理
	if (KeyA.pressed())
		radians -= pJet_speed * deltaTime;
	if (KeyD.pressed())
		radians += pJet_speed * deltaTime;
	pJet_pos = OffsetCircular({ 0,0 }, pJet_r, radians);
	pJet_collider.setCenter(pJet_pos);
	shieldCollider.setCenter(pJet_pos);

	//p弾発射処理
	if (!shieldFlag)
	{
		if ((pShotCoolTime < pShotTimer) && KeyJ.pressed())
		{
			pShotTimer = fmod(pShotTimer, pShotCoolTime);
			pBullet_posArr << Vec2{ pJet_r + 10 ,radians };
			pBullet_coliArr << Circle{ 0,0,pBullet_r };
			pShotAud.playOneShot();
		}
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
		//移動処理
		enemy.move();
		//発射処理
		enemy.eShotTimer += deltaTime;
		if ((eShotCoolTime < enemy.eShotTimer))
		{
			enemy.eShotTimer = fmod(enemy.eShotTimer, eShotCoolTime);
			Vec2 direction = pJet_pos - enemy.getCollider().center;
			eBulletArr << Bullet{ Circle{0,0,eBullet_r},Vec2{enemy.getCollider().center},direction};
		}
	}
	//E弾処理
	//移動処理
	for (auto& bullet : eBulletArr)
	{
		bullet.position += (bullet.direction * eBullet_speed * deltaTime);
		bullet.collider.setCenter(bullet.position);
	}
	//e弾hit
	for (auto it = eBulletArr.begin(); it != eBulletArr.end();)
	{
		bool exsit = false;
		for (auto& town : townArr)
		{
			if (it->collider.intersects(town.collider))
			{
				town.townHP -= eBullet_damage;
				it = eBulletArr.erase(it);
				exsit = true;
				break;
			}
		}
		if (exsit)
			continue;
		if (calcPJetHP(it->collider, pJet_collider))
		{
			it = eBulletArr.erase(it);
		}
		else
		{
			if (it->collider.intersects(earth))
				it = eBulletArr.erase(it);
			else
				++it;
		}
	}
	//範囲外の弾は削除
	eBulletArr.remove_if([](const Bullet& b) {return (b.position.x < -1000)||(b.position.x > 1000)|| (b.position.y < -1000) || (b.position.y > 1000); });
	//シールド判定
	if(shieldFlag)
		eBulletArr.remove_if([s = shieldCollider](const Bullet& b) {return b.collider.intersects(s); });

	//Town処理
	for (auto& town : townArr)
	{
		if (town.townHP <= 0)
		{
			gameOverFlag = true;
		}
	}

	//カメラ計算
	camera.setTargetCenter({0,-pJet_r-80});
	mat = Mat3x2::Rotate(-radians, {0,0});
	camera.update();
}




void Game::draw() const
{
	Scene::SetBackground(HSV{ 267, 0.99, 0.25 });
	{
		// 2D カメラの設定から Transformer2D を作成
		const auto t0 = camera.createTransformer();
	
		const Transformer2D t1{ mat,TransformCursor::Yes };

		//ステージ
		earth.draw(Palette::Brown);
		house.scaled(0.8).drawAt(0, -earth_r);
		house.scaled(0.8).rotated(Math::HalfPi).drawAt(earth_r, 0);
		house.scaled(0.8).rotated(Math::Pi).drawAt(0, earth_r);
		house.scaled(0.8).rotated(-Math::HalfPi).drawAt(-earth_r, 0);

		//プレイヤー
		if(shieldFlag)
			shieldTex.scaled(1.5).drawAt(pJet_pos);	
		pJetTex.rotated(radians).drawAt(pJet_pos);
		
		//p弾
		for (auto& bullet : pBullet_coliArr)
		{
			bullet.draw(Palette::Black);
			pBullet_tex.rotated(radians).drawAt(bullet.center);
		}
		//敵
		for (auto& enemy : enemy_arr)
		{
			if (enemy.currentHP > 0)
			{
				enemy.draw();
			}
		}
		//敵弾
		for (auto& eBullet : eBulletArr)
		{
			eBullet_tex.drawAt(eBullet.position);
		}
	}
	if (gameOverFlag)
	{
		font(U"GMAE OVER").drawAt(Scene::Center(), Palette::Gray);
		font(U"Press J Key").drawAt(20,{ Scene::Center().x,350 }, Palette::Gray);
	}
}
