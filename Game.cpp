#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
	townArr << Town{ town0,townHP } << Town{ town1,townHP } << Town{ town2,townHP } << Town{ town3,townHP };
	pJet_HP = pJet_MaxHP;
	if (not enemyCSV) // もし読み込みに失敗したら
	{
		throw Error{ U"Failed to load `EnemyDataSheat.csv`" };
	}
	for (size_t i = 1; i < enemyCount; i++)
	{
		ReadEnemyData readLine;
		readLine.enemyType = Parse<int>(enemyCSV[i][0]);
		readLine.movePattern = Parse<int>(enemyCSV[i][1]);
		readLine.shotPattern = Parse<int>(enemyCSV[i][2]);
		readLine.spawnTime = Parse<int>(enemyCSV[i][3]);
		readLine.returnTime = Parse<int>(enemyCSV[i][4]);
		readLine.r = Parse<double>(enemyCSV[i][5]);
		readLine.deg = Parse<double>(enemyCSV[i][6]);
		readEnemyDataArr << readLine;
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
		gameOverFlag = true;
	//ゲームオーバー
	if (gameOverFlag)
	{
		if (KeyJ.down())
			changeScene(State::Title);
		return;
	}
	
	//-------プレイヤー-------
	//p操作受付
	shieldFlag = KeyK.pressed();
	if (KeyA.pressed() || KeyLeft.pressed())
		radians -= horizSpeed * deltaTime;
	if (KeyD.pressed() || KeyRight.pressed())
		radians += horizSpeed * deltaTime;
	if (KeyW.pressed() || KeyUp.pressed())
		pJet_r += vertSpeed * deltaTime;
	if (KeyS.pressed() || KeyDown.pressed())
		pJet_r -= vertSpeed * deltaTime;
	pJet_r = Clamp(pJet_r, moveRange.bottom, moveRange.top);

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
			if (!it->getDeathFlag() && (pBullet_coliArr.at(i).intersects(it->getCollider())))
			{
				pBullet_coliArr.erase(pBullet_coliArr.begin() + i);
				pBullet_posArr.erase(pBullet_posArr.begin() + i);
				i--;
				if (it->calcHP(pBullet_damage))
				{
					int rondomNum = Random(0, 10);
					if (rondomNum < 3)
						itemArr << Item{ rondomNum,it->r_deg };
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
	double  maxShieldHealth = baseShieldHealth + pUpgrade.at(2) * shieldHealthRate;
	//自然回復
	shieldHealth = std::min(maxShieldHealth, shieldHealth + shieldRegenerationRate * deltaTime);
	//e弾処理
	if (shieldFlag && shieldHealth > 0)
	{
		Circle shieldCollider{ pJet_collider.center,shieldSize * 30.0 };
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
		if (it->r_deg.x > moveRange.bottom)
			it->r_deg.x -= itemSpeed * deltaTime;
		Vec2 rectPos = OffsetCircular({ 0,0 }, it->r_deg.x, it->r_deg.y * Math::Pi / 180);
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
			Vec2 temp{ Random(10,30),Random(0,360) };
			enemy_arr << Enemy{ temp ,getData().earth_r,getData().enemyHouseRange };
			spawnNum--;
		}
	}

	//CSVスポーン
	while (addLine + 1 <= readEnemyDataArr.size())
	{
		if (readEnemyDataArr.at(addLine).spawnTime < sceneTime)
		{
			enemy_arr << Enemy{ readEnemyDataArr.at(addLine),getData().earth_r,getData().enemyHouseRange };
			addLine++;
		}
		else
			break;
	}
	//e本体処理
	for (auto it = enemy_arr.begin(); it != enemy_arr.end();)
	{
		if (it->getDeathFlag())
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
			it->Shot(eBulletArr, pJet_pos);
		}
		++it;
	}
	//E弾処理
	//移動処理
	for (auto& bullet : eBulletArr)
	{
		Vec2 move(bullet.direction * eBullet_speed * deltaTime);
		bullet.collider.setCenter(bullet.collider.center + move);
	}
	//e弾hit
	for (auto it = eBulletArr.begin(); it != eBulletArr.end();)
	{
		bool exsit = false;
		for (auto i : step(4))
		{
			if (it->collider.intersects(townArr.at(i).collider))
			{
				if(!getData().testMode)
					townArr.at(i).townHP -= eBullet_damage;
				hpBars[i].damage(eBullet_damage);
				it = eBulletArr.erase(it);
				exsit = true;
				break;
			}
		}
		if (exsit)
			continue;

		if (it->collider.intersects(pJet_collider))
		{
			if (!getData().testMode)
				pJet_HP -= eBullet_damage;
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
	eBulletArr.remove_if([](const Bullet& b) {return (b.collider.x < -1000) || (b.collider.x > 1000) || (b.collider.y < -1000) || (b.collider.y > 1000); });

	//Town処理
	for (auto& town : townArr)
	{
		if (town.townHP <= 0)
		{
			gameOverFlag = true;
		}
	}
	//HPBar
	for (size_t i = 0; i < hpBars.size(); ++i)
	{
		hpBars[i].update();
	}

	//カメラ計算
	camera.setTargetCenter({ 0,-moveRange.bottom - 110 });
	camera.setTargetScale(cameraScale);
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
		earth.draw(Palette::Brown);
		for (int i = 0; i < 100; i++)
		{
			double tileDeg = Math::Pi * 2 / 100 * i;
			TextureAsset(U"earthTile").scaled(0.07).rotated(tileDeg).drawAt(OffsetCircular({0,0}, earth_r, tileDeg));
		}
		house.scaled(0.8).drawAt(0, -earth_r);
		house.scaled(0.8).rotated(Math::HalfPi).drawAt(earth_r, 0);
		house.scaled(0.8).rotated(Math::Pi).drawAt(0, earth_r);
		house.scaled(0.8).rotated(-Math::HalfPi).drawAt(-earth_r, 0);

		//プレイヤー
		if (shieldFlag && shieldHealth > 0)
		{
			Circle{ pJet_collider.center,shieldSize * 30.0 }.draw(Palette::Pink);
			shieldTex.scaled(1.8 * shieldSize).drawAt(pJet_pos);
		}
		pJetTex.scaled(playerSize).rotated(radians).drawAt(pJet_pos);
		
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
				enemy.explosion_Anime.draw(OffsetCircular({ 0,0 }, enemy.r_deg.x, enemy.r_deg.y * Math::Pi / 180));
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
			TextureAsset(texName).scaled(0.04).rotated(item.r_deg.y * Math::Pi / 180).drawAt(OffsetCircular({ 0,0 }, item.r_deg.x, item.r_deg.y * Math::Pi / 180));
		}
	}

	//-------UI------------
	//街のHP
	if (KeyH.pressed())
	{
		FontAsset(U"townHPTex")(U"普通の街 HP").drawAt(100, 60);
		FontAsset(U"townHPTex")(U"攻撃の街 HP").drawAt(300, 60);
		FontAsset(U"townHPTex")(U"防御の街 HP").drawAt(500, 60);
		FontAsset(U"townHPTex")(U"特殊の街 HP").drawAt(700, 60);

		RoundRect{ 10,15,180,80,10 }.draw(ColorF{ 1.0,0.4 });
		RoundRect{ 210,15,180,80,10 }.draw(ColorF{ 1.0,0.4 });
		RoundRect{ 410,15,180,80,10 }.draw(ColorF{ 1.0,0.4 });
		RoundRect{ 610,15,180,80,10 }.draw(ColorF{ 1.0,0.4 });
	}

	//HPBar
	for (size_t i = 0; i < hpBars.size(); i++)
	{
		const double x = i * 200;
		const double y = 30;
		const RectF rect = RectF{ x, y, 150, 16 }.movedBy(25, 0);
		hpBars[i].draw(rect);
	}
	//GameOver
	if (gameOverFlag)
	{
		font(U"GMAE OVER").drawAt(Scene::Center(), Palette::Gray);
		font(U"Press J Key").drawAt(20, { Scene::Center().x,350 }, Palette::Gray);
	}
}
