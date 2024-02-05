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
	//一時停止用
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
		if (shotInput.down())
		{
			changeScene(State::Title);
		}
		return;

	case Game::clear:
		if (shotInput.down())
		{
			changeScene(State::Title);
		}
		return;

	default:
		break;
	}

	//BGM再生
	AudioAsset(U"gameBGM").play();

	//ゲームシーンになった時、操作方法を表示
	if(showInstructionsFlag && !getData().testMode)
	{
		if (shotInput.pressed())
		{
			showInstructionsFlag = false;
		}
		return;
	}

	//時間管理
	deltaTime = Scene::DeltaTime();
	sceneTime += deltaTime;

	//状況に応じてゲームの状態を変更
	if (sceneTime > clearTime && !getData().testMode)
	{
		gameState = clear;
	}
	if (player.getHP() <= 0)
	{
		gameState = gameOver;
	}


	//----------------プレイヤー----------------

	//操作入力
	moveInput = { 0,0 };
	if (rightInput.pressed())
	{
		moveInput.x += 1.0;
	}
	if (leftInput.pressed())
	{
		moveInput.x -= 1.0;
	}
	if (upInput.pressed())
	{
		moveInput.y += 1.0;
	}
	if (downInput.pressed())
	{
		moveInput.y += -1.0;
	}
	moveInput.x += XInput(0).leftThumbX;
	moveInput.y += XInput(0).leftThumbY;
	
	moveInput = Vec2{ Clamp(moveInput.x ,-1.0,1.0) ,Clamp(moveInput.y ,-1.0,1.0) };
	player.move(moveInput);
	player.update(deltaTime);

	//攻撃orシールド展開
	player.useShield(shieldInput.pressed());

	if (shotInput.pressed() && !shieldInput.pressed())
	{
		player.shot(pBulletArr);
	}
	

	//弾の更新
	for (auto bulletIter = pBulletArr.begin(); bulletIter != pBulletArr.end();)
	{
		//移動
		Vec2 update(bulletIter->direction * pBullet_speed * deltaTime);
		Line trajectory{ bulletIter->collider.center,bulletIter->collider.center + update };
		bulletIter->collider.setCenter(bulletIter->collider.center + update);

		//弾自身が範囲外なら削除
		if (bulletIter->collider.x < -StageInfo::bulletDeleteRange || StageInfo::bulletDeleteRange < bulletIter->collider.x || bulletIter->collider.y < -StageInfo::bulletDeleteRange || bulletIter->collider.y > StageInfo::bulletDeleteRange)
		{
			bulletIter = pBulletArr.erase(bulletIter);
			continue;
		}

		//弾と敵の衝突処理
		bool isHit = false;
		for (auto enemyIter = eArr.begin(); enemyIter != eArr.end();)
		{
			if ((Geometry2D::Distance(trajectory, enemyIter->getCenter()) < bulletIter->collider.r + enemyIter->getCollider().r) && enemyIter->getHP() > 0)
			{
				switch (bulletIter->type)
				{
				case Normal:
					enemyIter->damage(bulletIter->damage);
					bulletIter = pBulletArr.erase(bulletIter);
					isHit = true;
					break;

				case Enhanced:
					if (!enemyIter->isHitThisBullet(bulletIter->ID))
					{
						enemyIter->damage(bulletIter->damage);
					}
					break;

				case TownBullet:
					enemyIter->damage(bulletIter->damage);
					bulletIter = pBulletArr.erase(bulletIter);
					isHit = true;
					break;

				default:
					break;
				}
			}
			enemyIter++;
			if (isHit)
			{
				break;
			}
		}
		if (!isHit)
		{
			bulletIter++;
		}
	}


	//-------------シールド処理------------------
	
	//e弾VS Shield
	if (player.isShieldAvailable())
	{
		for (auto it = eBulletArr.begin(); it != eBulletArr.end();)
		{
			if (it->type == BulletType::EnemyBullet && it->collider.intersects(player.getShieldCollider()))
			{
				player.shieldDamage(it->damage);
				player.addEnhancePoint(it->damage / 10);
				it = eBulletArr.erase(it);

				continue;
			}
			it++;
		}
	}


	//--------------アップグレードアイテム-------------------
	for (auto it = itemArr.begin(); it != itemArr.end();)
	{
		//アイテムの移動
		if (it->pos.r > StageInfo::earthR)
		{
			it->pos.r -= itemSpeed * deltaTime;
		}

		//衝突判定（プレイヤー）
		Vec2 rectPos = OffsetCircular({ 0,0 }, it->pos);
		Rect collider{ Arg::center(lround(rectPos.x),lround(rectPos.y)) ,20,20 };
		if (collider.intersects(player.getCollider()))
		{
			player.addOnePointUpgrade(it->itemType);
			it = itemArr.erase(it);
			continue;
		}
		it++;
	}
	for (auto& town:townArr)
	{
		if (town.getCollider().intersects(player.getCollider()))
		{
			town.addUpgrade(player.getUpgradeCnt());
			player.resetUpgrade();
		}
	}

	//---------------Enemy処理-----------------

	//ランダムスポーン
	eSpawnTimer += deltaTime;
	if (eSpawnTimer > spawnIntervalSeconds)
	{
		eSpawnTimer -= spawnIntervalSeconds;
		for(int i:step(enemySpawnCalc(sceneTime)))
		{
			const double r = Random(minSpawnR,maxSpawnR );
			const double theta = Math::ToRadians(Random(minSpawnTheta, maxSpawnTheta));
			eArr << Enemy{ r, theta };
		}
	}

	//TODO:CSVファイルが1行の時エラー,sceneTimeが100以上の時エラー
	//CSVスポーン １行目は項目のためスルー
	/*while (index + 2 <= enemyCSV.rows())
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
	}*/

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
			if (it->collider.intersects(townArr[i].getCollider()))
			{
				if (!getData().testMode)
				{
					townArr[i].damage(it->damage);
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
				player.damage(it->damage);
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
	eBulletArr.remove_if([](const Bullet& b) {return (b.collider.x < -StageInfo::bulletDeleteRange) || (b.collider.x > StageInfo::bulletDeleteRange) || (b.collider.y < -StageInfo::bulletDeleteRange) || (b.collider.y > StageInfo::bulletDeleteRange); });

	//town更新
	for (size_t i = 0; i < townArr.size(); ++i)
	{
		townArr.at(i).update(deltaTime);
		townArr[i].shot(pBulletArr);
	}

	//Town処理
	for (auto& town : townArr)
	{
		if (town.getHP() <= 0)
		{
			gameState = gameOver;
		}
	}


	//------------------カメラ計算------------------
	
	//引数の座標はゲーム内ではなく、回転の処理をした後、スケールを変える前の画面上座標
	camera.setTargetCenter(Circular{player.getR() + cameraOffsetY,0});
	if (cameraMode)
	{
		if (player.getR()< StageInfo::earthR)
		{
			camera.setTargetScale(cameraScale * (1 - 0.65 * ((StageInfo::earthR - player.getR()) / StageInfo::earthR)));
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
			TextureAsset(U"earthTile").scaled(0.07).rotated(tileDeg).drawAt(OffsetCircular({ 0,0 }, StageInfo::earthR, tileDeg));
		}

		//街
		for (int32 i:step(townArr.size()))
		{
			double townRotate = Math::ToRadians(i * (360/townArr.size()));
			switch (townArr[i].getTownType())
			{
			case TownType::Nomal:
				TextureAsset(U"townTex").scaled(0.2).rotated(townRotate).drawAt(Circular(StageInfo::earthR + townPosOffset.r, townRotate + townPosOffset.theta));
				break;
			case TownType::Attack:
				break;
			case TownType::Defense:
				break;
			case TownType::Special:
				break;
			default:
				break;
			}
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
			case TownBullet:
				TextureAsset(U"pBullet_tex").rotated(player.getTheta()).drawAt(bullet.collider.center);
				break;
			default:
				TextureAsset(U"townTex").rotated(player.getTheta()).drawAt(bullet.collider.center);
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
	//遊び方
	if (showInstructionsFlag && !getData().testMode)
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
		townArr.at(i).drawHPBar(rect);
	}

	//プレイヤー強化
	for (auto i : step(3))
	{
		Rect{ 810 + 100 * i,950,100,60 }.draw(Palette::Darkgray);
		Rect{ 810 + 100 * i,950,100,60 }.drawFrame(5, Palette::Black);
		switch (i)
		{
		case 0:
			FontAsset(U"townHPFont")(player.getUpgradeCnt().Attack).drawAt(860 + 100 * i, 980, Palette::Blue);
			break;
		case 1:
			FontAsset(U"townHPFont")(player.getUpgradeCnt().Defense).drawAt(860 + 100 * i, 980, Palette::Blue);
			break;
		case 2:
			FontAsset(U"townHPFont")(player.getUpgradeCnt().Special).drawAt(860 + 100 * i, 980, Palette::Blue);
			break;
		default:
			break;
		}
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
