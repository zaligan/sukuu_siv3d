#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
	townArr << Town{town0,townHP} << Town{ town1,townHP } << Town{ town2,townHP } << Town{ town3,townHP };
	pJet_HP = pJet_MaxHP;
	if (not enemyCSV) // もし読み込みに失敗したら
	{
		throw Error{ U"Failed to load `EnemyDataSheat.csv`" };
	}
	for (size_t i =1;i<enemyCount;i++)
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
	//ClearPrint();
	
	if (KeyP.pressed())
		return;

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
				i--;
				it->calcHP(pBullet_damage);
				++it;
				break;
			}
			else
			{
				++it;
			}
		}
	}
	//Enemy処理
	//スポーン
	while (addLine + 2 <= enemyCSV.rows())
	{
		if (readEnemyDataArr.at(addLine).spawnTime < sceneTime)
		{
			enemy_arr << Enemy{ readEnemyDataArr.at(addLine),getData().earth_r,getData().enemyHouseRange};
			addLine++;
		}
		else
			break;
	}
	
	for (auto it = enemy_arr.begin(); it != enemy_arr.end();)
	{
		if (it->currentHP <= 0)
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
		bullet.position += (bullet.direction * eBullet_speed * deltaTime);
		bullet.collider.setCenter(bullet.position);
	}
	//e弾hit
	for (auto it = eBulletArr.begin(); it != eBulletArr.end();)
	{
		bool exsit = false;
		for (auto i : step(4))
		{
			if (it->collider.intersects(townArr.at(i).collider))
			{
				townArr.at(i).townHP -= eBullet_damage;
				hpBars[i].damage(eBullet_damage);
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
	//HPBar
	for (size_t i = 0; i < hpBars.size(); ++i)
	{
		hpBars[i].update();
	}

	//カメラ計算
	camera.setTargetCenter({0,-pJet_r-80});
	mat = Mat3x2::Rotate(-radians, {0,0});
	camera.update();
}




void Game::draw() const
{
	{
		// 2D カメラの設定から Transformer2D を作成
		const auto t0 = camera.createTransformer();
		const Transformer2D t1{ mat,TransformCursor::Yes };

		backPic.scaled(1.0).drawAt(0,0);
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
			else
			{
				enemy.explosion_Anime.draw(OffsetCircular({ 0,0 }, enemy.r_deg.x, enemy.r_deg.y * Math::Pi /180));
			}
		}
		//敵弾
		for (auto& eBullet : eBulletArr)
		{
			eBullet_tex.drawAt(eBullet.position);
		}
	}
	//HPBar
	for (size_t i = 0; i < hpBars.size(); i++)
	{
		const double x =i * 200;
		const double y = 500;
		const RectF rect = RectF{ x, y, 150, 16 }.movedBy(25,0);
		hpBars[i].draw(rect);
	}
	//GameOver
	if (gameOverFlag)
	{
		font(U"GMAE OVER").drawAt(Scene::Center(), Palette::Gray);
		font(U"Press J Key").drawAt(20,{ Scene::Center().x,350 }, Palette::Gray);
	}
}
