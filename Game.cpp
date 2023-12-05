﻿#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
	pJet_HP = pJet_MaxHP;
	if (not enemyCSV) // もし読み込みに失敗したら
	{
		throw Error{ U"Failed to load `EnemyDataSheat.csv`" };
	}
}

void Game::update()
{
	gameBGM.play();
	//Print

	//時間管理
	deltaTime = Scene::DeltaTime();
	sceneTime += deltaTime;
	pShotTimer += deltaTime;
	eSpawnTimer += deltaTime;

	if (pJet_HP <= 0)
	{
		gameOverFlag = true;
	}
	//ゲームオーバー
	if (gameOverFlag)
	{
		if (KeyJ.down())
		{
			changeScene(State::Title);
		}
		return;
	}

	//-------プレイヤー-------
	//p操作受付
	shieldFlag = KeyK.pressed();
	if (KeyK.up())
	{
		shieldAnime.reset();
	}
	if (KeyA.pressed() || KeyLeft.pressed())
	{
		radians -= 2 * Math::Pi * deltaTime / (maxRotatSpeed + ((minRotatSpeed - maxRotatSpeed) * ((pJet_r - moveRange.minRadius) / (moveRange.maxRadius - moveRange.minRadius))));
	}
	if (KeyD.pressed() || KeyRight.pressed())
	{
		radians += 2 * Math::Pi * deltaTime / (maxRotatSpeed + ((minRotatSpeed - maxRotatSpeed) * ((pJet_r - moveRange.minRadius) / (moveRange.maxRadius - moveRange.minRadius))));
	}
	if (KeyW.pressed() || KeyUp.pressed())
	{
		pJet_r += vertSpeed * deltaTime;
	}
	if (KeyS.pressed() || KeyDown.pressed())
	{
		pJet_r -= vertSpeed * deltaTime;
	}
	//p移動
	pJet_r = Clamp(pJet_r, moveRange.minRadius, moveRange.maxRadius);
	pJet_pos = OffsetCircular({ 0,0 }, pJet_r, radians);
	pJet_collider.setCenter(pJet_pos);

	//p弾発射処理
	if (!shieldFlag)
	{
		double coolTime = pShotCoolTime * std::pow(shotSpeedRate, pUpgrade.at(0));
		if ((coolTime < pShotTimer) && KeyJ.pressed())
		{
			pShotTimer = 0;
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
			if (!it->checkDeath() && (pBullet_coliArr.at(i).intersects(it->getCollider())))
			{
				pBullet_coliArr.erase(pBullet_coliArr.begin() + i);
				pBullet_posArr.erase(pBullet_posArr.begin() + i);
				i--;
				if (it->calcHP(pBullet_damage))
				{
					int rondomNum = Random(0, 10);
					if (rondomNum < 3)
					{
						itemArr << Item{ rondomNum,it->r_deg };
					}
				}
				++it;
				break;
			}
			else
			{
				++it;
			}
		}
	}

	//----シールド処理------
	//アップグレード強化
	maxShieldHealth = baseShieldHealth + pUpgrade.at(2) * shieldUpgRate;
	//自然回復
	shieldHealth = std::min(maxShieldHealth, shieldHealth + shieldRegenerationRate * deltaTime);
	//e弾処理
	if (shieldFlag && shieldHealth > 0)
	{
		Circle shieldCollider{ pJet_collider.center,shieldSize * 30.0 };
		shieldAnime.update();
		for (auto it = eBulletArr.begin(); it != eBulletArr.end();)
		{
			if (it->collider.intersects(shieldCollider))
			{
				shieldHealth = std::max(-shieldRegenerationRate, shieldHealth - eBullet_damage);
				it = eBulletArr.erase(it);
				continue;
			}
			it++;
		}
		eBulletArr.remove_if([s = shieldCollider](const Bullet& b) {return b.collider.intersects(s); });

	}


	//-----Item処理------
	for (auto it = itemArr.begin(); it != itemArr.end();)
	{
		if (it->r_deg.x > earth_r)
		{
			it->r_deg.x -= itemSpeed * deltaTime;
		}
		Vec2 rectPos = OffsetCircular({ 0,0 }, it->r_deg.x, it->r_deg.y);
		Rect collider{ Arg::center(lround(rectPos.x),lround(rectPos.y)) ,20,20 };
		if (collider.intersects(pJet_collider))
		{
			pUpgrade.at(it->itemType)++;
			it = itemArr.erase(it);
			continue;
		}
		it++;
	}

	//------Enemy処理--------

	//ランダムスポーン
	spawnTimer += deltaTime;
	if (spawnTimer > 1.0)
	{
		spawnTimer -= 1.0;
		spawnNum += spawnRate;
		while (1.0 <= spawnNum)
		{
			const double r = Random(10, 30);
			const double degree = Math::ToRadians(Random(0, 360));
			enemy_arr << Enemy{ r, degree ,earth_r,enemyHouseRange };
			spawnNum--;
		}
	}

	//CSVスポーン １行目は項目のためスルー
	while (index + 2 <= enemyCSV.rows())
	{
		if (Parse<int32>(enemyCSV[index + 1][0]) < sceneTime)
		{
			const double r = Parse<double>(enemyCSV[index + 1][1]);
			const double degree = Math::ToRadians(Parse<double>(enemyCSV[index + 1][2]));
			enemy_arr << Enemy{ r, degree, earth_r, enemyHouseRange };
			index++;
		}
		else
		{
			break;
		}
	}

	//e本体処理
	for (auto it = enemy_arr.begin(); it != enemy_arr.end();)
	{
		if (it->checkDeath())
		{
			if (it->explosion_Anime.update())
			{
				it = enemy_arr.erase(it);
				continue;
			}
		}
		//移動処理
		it->move();
		//発射処理
		it->eShotTimer += deltaTime;
		if ((it->geteShotCoolTime() < it->eShotTimer))
		{
			it->shot(eBulletArr, pJet_pos);
		}
		++it;
	}
	//E弾処理
	//移動処理
	for (auto& bullet : eBulletArr)
	{
		// TODO: bullet.direction を単位ベクトルにしたので、eBullet_speed の値は再検討が必要
		Vec2 move(bullet.direction * eBullet_speed * deltaTime);
		bullet.collider.setCenter(bullet.collider.center + move);
	}
	//e弾hit
	for (auto it = eBulletArr.begin(); it != eBulletArr.end();)
	{
		bool exsit = false;
		for (auto i : step(townArr.size()))
		{
			if (it->collider.intersects(townArr.at(i).collider))
			{
				if (!getData().testMode)
					townArr.at(i).hp.damage(eBullet_damage);
				it = eBulletArr.erase(it);
				exsit = true;
				break;
			}
		}
		if (exsit)
		{
			continue;
		}

		if (it->collider.intersects(pJet_collider))
		{
			if (!getData().testMode)
			{
				pJet_HP -= eBullet_damage;
			}
			it = eBulletArr.erase(it);
		}

		else
		{
			if (it->collider.intersects(earth))
			{
				it = eBulletArr.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	//範囲外の弾は削除
	eBulletArr.remove_if([](const Bullet& b) {return (b.collider.x < -1000) || (b.collider.x > 1000) || (b.collider.y < -1000) || (b.collider.y > 1000); });

	//Town処理
	for (auto& town : townArr)
	{
		if (town.hp.getHP() <= 0)
		{
			gameOverFlag = true;
		}
	}
	//HPBar
	for (size_t i = 0; i < townArr.size(); ++i)
	{
		townArr.at(i).hp.update();
	}

	//カメラ計算
	//引数の座標はゲーム内ではなく、回転の処理をした後、スケールを変える前の画面上座標
	camera.setTargetCenter({ 0,-pJet_r - 90 });

	if (pJet_r < earth_r)
	{
		camera.setTargetScale(cameraScale * (1 - 0.65 * ((earth_r - pJet_r) / earth_r)));
	}
	else
	{
		camera.setTargetScale(cameraScale);
	}

	mat = Mat3x2::Rotate(-radians, { 0,0 });
	camera.update();
}




void Game::draw() const
{
	{
		// 2D カメラの設定から Transformer2D を作成
		const auto t0 = camera.createTransformer();
		const Transformer2D t1{ mat,TransformCursor::Yes };

		backPic.scaled(1.0).drawAt(0, 0);
		//ステージ
		earth.draw(Palette::Saddlebrown);
		for (int i = 0; i < 100; i++)
		{
			double tileDeg = Math::Pi * 2 / 100 * i;
			TextureAsset(U"earthTile").scaled(0.07).rotated(tileDeg).drawAt(OffsetCircular({ 0,0 }, earth_r, tileDeg));
		}
		house.scaled(0.8).drawAt(0, -earth_r);
		house.scaled(0.8).rotated(Math::HalfPi).drawAt(earth_r, 0);
		house.scaled(0.8).rotated(Math::Pi).drawAt(0, earth_r);
		house.scaled(0.8).rotated(-Math::HalfPi).drawAt(-earth_r, 0);

		//プレイヤー
		pJetTex.scaled(playerSize).rotated(radians).drawAt(pJet_pos);
		if (shieldFlag && shieldHealth > 0)
		{
			double colorH = (maxShieldHealth - shieldHealth) / maxShieldHealth * 110;
			Circle{ pJet_collider.center,shieldSize * 30.0 }.draw(ColorF(HSV{ 250 + colorH,0.9,1 }, 0.7));
			shieldAnime.drawAt(pJet_pos, radians);
		}


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
			else
			{
				enemy.explosion_Anime.drawAt(OffsetCircular({ 0,0 }, enemy.r_deg.x, enemy.r_deg.y));
			}
		}
		//敵弾
		for (auto& eBullet : eBulletArr)
		{
			eBullet_tex.drawAt(eBullet.collider.center);
		}
		//アイテム
		for (auto& item : itemArr)
		{

			String texName;
			switch (item.itemType)
			{
			case 0:
				texName = U"Attack_Item";
				break;
			case 1:
				texName = U"Protect_Item";
				break;
			case 2:
				texName = U"Special_Item";
				break;
			default:
				break;
			}
			TextureAsset(texName).scaled(0.04).rotated(item.r_deg.y).drawAt(OffsetCircular({ 0,0 }, item.r_deg.x, item.r_deg.y));
		}
	}

	//-------UI------------
	//街のHP
	double interval = 75;
	Array<String> townNameArr = { U"普通の街 HP",U"攻撃の街 HP" ,U"防御の街 HP" ,U"特殊の街 HP" };
	for (int i = 0; i < townArr.size(); i++)
	{
		RoundRect{ 10,(i * interval) + 15,180,60,10 }.draw(ColorF{ 1.0,0.4 });
		FontAsset(U"townHPFont")(townNameArr.at(i)).drawAt(100, (i * interval) + 60);
	}
	for (size_t i = 0; i < townArr.size(); i++)
	{
		const double x = 0;
		const double y = interval * i;
		const RectF rect = RectF{ x, y, 150, 16 }.movedBy(25, 30);
		townArr.at(i).hp.draw(rect);
	}

	//プレイヤー強化
	for (auto i : step(pUpgrade.size()))
	{
		Rect{ 810 + 100 * i,1020,100,60 }.draw(Palette::Darkgray);
		Rect{ 810 + 100 * i,1020,100,60 }.drawFrame(5, Palette::Black);
		FontAsset(U"townHPFont")(pUpgrade.at(i)).drawAt(860 + 100 * i, 1050, Palette::Blue);
	}
	TextureAsset(U"Attack_Item").scaled(0.05).drawAt(830, 1050);
	TextureAsset(U"Protect_Item").scaled(0.05).drawAt(930, 1050);
	TextureAsset(U"Special_Item").scaled(0.05).drawAt(1030, 1050);

	//GameOver
	if (gameOverFlag)
	{
		font(U"GMAE OVER").drawAt(Scene::Center(), Palette::Gray);
		font(U"Press J Key").drawAt(20, { Scene::Center().x,350 }, Palette::Gray);
	}

	//testモード
	if (getData().testMode)
	{
		RoundRect{ 1770,0,150,80,10 }.draw(ColorF{ Palette::Magenta ,0.4 });
		FontAsset(U"townHPFont")(U"test").drawAt(1845, 40);
	}
}
