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
	if (KeyP.pressed())
	{
		return;
	}

	//ゲームの状態遷移
	switch (gameState)
	{
	case Game::play:
		break;

	case Game::gameOver:
		if (KeyJ.down())
		{
			changeScene(State::Title);
		}
		return;

	case Game::clear:
		if (KeyJ.down())
		{
			changeScene(State::Title);
		}
		return;

	default:
		break;
	}

	AudioAsset(U"gameBGM").play();

	if(showInstructionsFlag)
	{
		if (KeyJ.pressed())
		{
			showInstructionsFlag = false;
		}
		return;
	}

	//時間管理
	deltaTime = Scene::DeltaTime();
	sceneTime += deltaTime;

	if (sceneTime > clearTime)
	{
		gameState = clear;
	}

	if (player.getHP() <= 0)
	{
		gameState = gameOver;
	}

	//-------プレイヤー-------
	//p操作受付
	player.update(deltaTime);

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
	//攻撃orシールド展開
	player.useShield(KeyK.pressed());

	if (KeyJ.pressed() && !KeyK.pressed())
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
				switch (bulletIter->type)
				{
				case Normal:
					enemyIter->damage(pBulletDamage);
					bulletIter = pBulletArr.erase(bulletIter);					
					break;

				case Enhanced:
					if (!enemyIter->isHitThisBullet(bulletIter->ID))
					{
						enemyIter->damage(pEnhancedBulletDamage);
					}
					bulletIter++;
					break;

				default:
					break;
				}

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

	//----シールド処理------
	//アップグレード強化
	//maxShieldHealth = baseShieldHealth + pUpgrade.at(2) * shieldUpgRate;
	
	//e弾VS Shield
	if (player.isShieldAvailable())
	{
		for (auto it = eBulletArr.begin(); it != eBulletArr.end();)
		{
			if (it->collider.intersects(player.getShieldCollider()))
			{
				player.shieldDamage(eBulletDamage);
				player.addEnhancePoint(eBulletDamage / 10);
				it = eBulletArr.erase(it);

				continue;
			}
			it++;
		}
	}


	//-----Item処理------
	for (auto it = itemArr.begin(); it != itemArr.end();)
	{
		if (it->pos.r > earthR)
		{
			it->pos.r -= itemSpeed * deltaTime;
		}
		Vec2 rectPos = OffsetCircular({ 0,0 }, it->pos);
		Rect collider{ Arg::center(lround(rectPos.x),lround(rectPos.y)) ,20,20 };
		if (collider.intersects(player.getCollider()))
		{
			pUpgrade.at(it->itemType)++;
			it = itemArr.erase(it);
			continue;
		}
		it++;
	}

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
			int32 itemDropChance = Random(0, 99);
			if (itemDropChance % 10 < 3)
			{
				itemArr << Item{ itemDropChance % 10,it->getCenter()};
			}
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
		Vec2 update(bullet.direction * eBulletSpeed * deltaTime);
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
				{
					townArr.at(i).hp.damage(eBulletDamage);
				}
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
				player.damage(eBulletDamage);
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
			gameState = gameOver;
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

		TextureAsset(U"backPic").scaled(1.0).drawAt(0, 0);
		//ステージ
		earth.draw(Palette::Saddlebrown);
		for (int i = 0; i < 100; i++)
		{
			double tileDeg = Math::Pi * 2 / 100 * i;
			TextureAsset(U"earthTile").scaled(0.07).rotated(tileDeg).drawAt(OffsetCircular({ 0,0 }, earthR, tileDeg));
		}

		for (int32 i:step(townArr.size()))
		{
			double townRotate = Math::ToRadians(i * 90);
			TextureAsset(U"townTex").scaled(0.2).rotated(townRotate).drawAt(Circular(earthR + townPosOffset.r,townRotate + townPosOffset.theta));
		}

		//プレイヤー
		player.draw();

		//p弾
		for (auto& bullet : pBulletArr)
		{
			switch (bullet.type)
			{
			case Normal:
				TextureAsset(U"pBullet_tex").rotated(player.getTheta()).drawAt(bullet.collider.center);
				break;
			case Enhanced:
				TextureAsset(U"pEnhancedBullet_tex").rotated(player.getTheta()-90_deg).drawAt(bullet.collider.center);
				break;
			default:
				break;
			}
		}
		//敵
		for (auto& enemy : eArr)
		{
			enemy.draw();
		}
		//敵弾
		for (auto& eBullet : eBulletArr)
		{
			TextureAsset(U"eBullet_tex").drawAt(eBullet.collider.center);
		}
		//アイテム
		for (auto& item : itemArr)
		{

			String itemType;
			switch (item.itemType)
			{
			case 0:
				itemType = U"Attack_Item";
				break;
			case 1:
				itemType = U"Protect_Item";
				break;
			case 2:
				itemType = U"Special_Item";
				break;
			default:
				break;
			}
			TextureAsset(itemType).scaled(0.04).rotated(item.pos.theta).drawAt(OffsetCircular({ 0,0 }, item.pos));
		}
	}

	//-------UI------------

	if (showInstructionsFlag)
	{
		TextureAsset(U"howToPlayTex").scaled(1.3).drawAt(Scene::Center());
	}

	//残り時間
	FontAsset(U"townHPFont")(U"{:.0f}"_fmt(Max(0.0, clearTime - sceneTime) )).drawAt(80,Scene::Center().x, 80);

	//街のHP
	double interval = 180;
	Array<String> townNameArr = { U"普通の街 HP",U"攻撃の街 HP" ,U"防御の街 HP" ,U"特殊の街 HP" };
	for (int i = 0; i < townArr.size(); i++)
	{
		RoundRect{ i*interval + 600,1020,180,60,10 }.draw(Palette::Gray);
		FontAsset(U"townHPFont")(townNameArr.at(i)).drawAt(i * interval + 690, 1030);
	}
	for (size_t i = 0; i < townArr.size(); i++)
	{
		const double x = interval * i;
		const double y = 0;
		const RectF rect = RectF{ x, y, 150, 16 }.movedBy(615, 1050);
		townArr.at(i).hp.draw(rect);
	}

	//プレイヤー強化
	for (auto i : step(pUpgrade.size()))
	{
		Rect{ 810 + 100 * i,950,100,60 }.draw(Palette::Darkgray);
		Rect{ 810 + 100 * i,950,100,60 }.drawFrame(5, Palette::Black);
		FontAsset(U"townHPFont")(pUpgrade.at(i)).drawAt(860 + 100 * i, 980, Palette::Blue);
	}
	TextureAsset(U"Attack_Item").scaled(0.05).drawAt(830, 980);
	TextureAsset(U"Protect_Item").scaled(0.05).drawAt(930, 980);
	TextureAsset(U"Special_Item").scaled(0.05).drawAt(1030, 980);

	//GameOver
	switch (gameState)
	{
	case Game::play:
		break;
	case Game::gameOver:
		font(U"GMAE OVER").drawAt(Scene::Center(), Palette::Gray);
		font(U"Press J Key").drawAt(40, { Scene::Center().x,Scene::Center().y + 100 }, Palette::Gray);
		break;
	case Game::clear:
		font(U"CLEAR!").drawAt(Scene::Center(), Palette::Gray);
		font(U"Press J Key").drawAt(40, { Scene::Center().x,Scene::Center().y + 100 }, Palette::Gray);
		break;
	default:
		break;
	}

	//testモード
	if (getData().testMode)
	{
		RoundRect{ 1770,0,150,80,10 }.draw(ColorF{ Palette::Magenta ,0.4 });
		FontAsset(U"townHPFont")(U"test").drawAt(1845, 40);
	}
}
