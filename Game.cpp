#include "Game.h"

// ゲームシーン
Game::Game(const InitData& init)
	: IScene{ init }
{
	
	if (not enemyCSV) // もし読み込みに失敗したら
	{
		throw Error{ U"Failed to load `EnemyDataSheat.csv`" };
	}
}

void Game::update()
{

	gameBGM.play();

	if (KeyP.pressed())
	{
		return;
	}

	//時間管理
	deltaTime = Scene::DeltaTime();
	sceneTime += deltaTime;

	player.update(deltaTime);

	if (player.getHP() <= 0)
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
	
	if (KeyA.pressed() || KeyLeft.pressed())
	{
		player.move(Directions::Left);
	}
	if (KeyD.pressed() || KeyRight.pressed())
	{
		player.move(Directions::Right);
	}
	if (KeyW.pressed() || KeyUp.pressed())
	{
		player.move(Directions::Up);
	}
	if (KeyS.pressed() || KeyDown.pressed())
	{
		player.move(Directions::Down);
	}
	//シールド
	player.useShield(KeyK.pressed());

	//p弾発射処理
	if (KeyJ.pressed())
	{
		player.shot(pBulletArr);
	}

	//弾の更新
	for (auto bulletIter = pBulletArr.begin(); bulletIter != pBulletArr.end();)
	{
		//移動
		Vec2 update(bulletIter->direction * pBullet_speed  * deltaTime);
		bulletIter->collider.setCenter(bulletIter->collider.center + update);

		//弾自身が範囲外なら削除
		if (bulletIter->collider.x < -bulletDeleteRange || bulletDeleteRange < bulletIter->collider.x || bulletIter->collider.y < -bulletDeleteRange || bulletIter->collider.y > bulletDeleteRange)
		{
			bulletIter = pBulletArr.erase(bulletIter);
			continue;
		}

		//弾と敵の衝突処理
		bool isHit = false;
		for (auto enemyIter = eArr.begin(); enemyIter != eArr.end();)
		{
			if (bulletIter->collider.intersects(enemyIter->getCollider()))
			{
				enemyIter->damage(pBulletDamage);
				bulletIter = pBulletArr.erase(bulletIter);
				isHit = true;
				break;
			}
			else
			{
				enemyIter++;
			}
		}
		if (!isHit)
		{
			bulletIter++;
		}
	}

	////----シールド処理------
	////アップグレード強化
	//maxShieldHealth = baseShieldHealth + pUpgrade.at(2) * shieldUpgRate;
	////自然回復
	//shieldHealth = std::min(maxShieldHealth, shieldHealth + shieldRegenerationRate * deltaTime);
	////e弾処理
	//if (shieldFlag && shieldHealth > 0)
	//{
	//	Circle shieldCollider{ pJet_collider.center,shieldSize * 30.0 };
	//	shieldAnime.update();
	//	for (auto it = eBulletArr.begin(); it != eBulletArr.end();)
	//	{
	//		if (it->collider.intersects(shieldCollider))
	//		{
	//			shieldHealth = std::max(-shieldRegenerationRate, shieldHealth - eBullet_damage);
	//			it = eBulletArr.erase(it);
	//			continue;
	//		}
	//		it++;
	//	}
	//	eBulletArr.remove_if([s = shieldCollider](const Bullet& b) {return b.collider.intersects(s); });

	//}


	////-----Item処理------
	//for (auto it = itemArr.begin(); it != itemArr.end();)
	//{
	//	if (it->pos.r > earthR)
	//	{
	//		it->pos.r -= itemSpeed * deltaTime;
	//	}
	//	Vec2 rectPos = OffsetCircular({ 0,0 }, it->pos);
	//	Rect collider{ Arg::center(lround(rectPos.x),lround(rectPos.y)) ,20,20 };
	//	if (collider.intersects(pJet_collider))
	//	{
	//		pUpgrade.at(it->itemType)++;
	//		it = itemArr.erase(it);
	//		continue;
	//	}
	//	it++;
	//}

	//------Enemy処理--------

	//ランダムスポーン
	eSpawnTimer += deltaTime;
	if (eSpawnTimer > spawnIntervalSeconds)
	{
		eSpawnTimer -= spawnIntervalSeconds;
		for(int i:step(spawnCnt))
		{
			const double r = Random(minSpawnR,maxSpawnR );
			const double theta = Math::ToRadians(Random(minSpawnTheta, maxSpawnTheta));
			eArr << Enemy{ r, theta };
		}
	}

	//TODO:CSVファイルが1行の時のエラー対応
	//CSVスポーン １行目は項目のためスルー
	while (index + 2 <= enemyCSV.rows())
	{
		if (Parse<int32>(enemyCSV[index + 1][0]) < sceneTime)
		{
			const double r = Parse<double>(enemyCSV[index + 1][1]);
			const double theta = Math::ToRadians(Parse<double>(enemyCSV[index + 1][2]));
			eArr << Enemy{ r, theta };
			index++;
		}
		else
		{
			break;
		}
	}

	//e本体処理
	for (auto it = eArr.begin(); it != eArr.end();)
	{
		if (it->isDeath())
		{
			it = eArr.erase(it);
			continue;
		}
		//移動処理
		it->update();
		//発射処理
		it->shot(eBulletArr, player.getCenter());

		++it;
	}
	//E弾処理
	//移動処理
	for (auto& bullet : eBulletArr)
	{
		// TODO: bullet.direction を単位ベクトルにしたので、eBullet_speed の値は再検討が必要
		Vec2 update(bullet.direction * eBullet_speed * deltaTime);
		bullet.collider.setCenter(bullet.collider.center + update);
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

		if (it->collider.intersects(player.getCollider()))
		{
			if (!getData().testMode)
			{
				player.damage(eBullet_damage);
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
	eBulletArr.remove_if([](const Bullet& b) {return (b.collider.x < -bulletDeleteRange) || (b.collider.x > bulletDeleteRange) || (b.collider.y < -bulletDeleteRange) || (b.collider.y > bulletDeleteRange); });

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
	camera.setTargetCenter(Circular{player.getR() + cameraOffsetY,0});
	if (cameraMode)
	{
		if (player.getR()< earthR)
		{
			camera.setTargetScale(cameraScale * (1 - 0.65 * ((earthR - player.getR()) / earthR)));
		}
		else
		{
			camera.setTargetScale(cameraScale);
		}
	}

	mat = Mat3x2::Rotate(-player.getTheta(), {0,0});
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
			TextureAsset(U"earthTile").scaled(0.07).rotated(tileDeg).drawAt(OffsetCircular({ 0,0 }, earthR, tileDeg));
		}
		house.scaled(0.8).drawAt(0, -earthR);
		house.scaled(0.8).rotated(Math::HalfPi).drawAt(earthR, 0);
		house.scaled(0.8).rotated(Math::Pi).drawAt(0, earthR);
		house.scaled(0.8).rotated(-Math::HalfPi).drawAt(-earthR, 0);

		//プレイヤー
		pJetTex.scaled(playerSize).rotated(player.getTheta()).drawAt(player.getCenter());
		/*if (shieldFlag && shieldHealth > 0)
		{
			double colorH = (maxShieldHealth - shieldHealth) / maxShieldHealth * 110;
			Circle{ pJet_collider.center,shieldSize * 30.0 }.draw(ColorF(HSV{ 250 + colorH,0.9,1 }, 0.7));
			shieldAnime.drawAt(OffsetCircular({ 0,0 },pJet_r + shieldAnimePosOffset.x,radians + shieldAnimePosOffset.y), radians);
		}*/


		//p弾
		for (auto& bullet : pBulletArr)
		{
			TextureAsset(U"pBullet_tex").rotated(player.getTheta()).drawAt(bullet.collider.center);
		}
		//敵
		for (auto& enemy : eArr)
		{
			enemy.draw();
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
			TextureAsset(texName).scaled(0.04).rotated(item.pos.theta).drawAt(OffsetCircular({ 0,0 }, item.pos));
		}
		//ピンク線
		for (int i : step(4))
		{
			Circular c1{ earthR,i * 90_deg };
			Line{ c1,OffsetCircular({c1},400,Math::ToRadians(60 + 90 * i))}.draw(Palette::Pink);
			Line{ c1,OffsetCircular({c1},400,Math::ToRadians(-60 + 90 * i))}.draw(Palette::Pink);
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
