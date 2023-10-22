# include <Siv3D.hpp> // Siv3D v0.6.12

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });
	const Texture houseTex{ U"🏚️"_emoji };
	const Texture pJetTex{ U"picture/pJetTex.png"};
	const double pJetScale = 0.02;
	const double pSpeed = 300.0;//プレイヤーの移動速度
	const int sidePanelWidth = 200.0;//両脇パネルの横幅
	Vec2 pPos{ 300,300 };
	Circle pColi{ 0,0,33};

	const Rect ground{ 0,550,800,50 };
	const Rect leftWind{ 0,0,sidePanelWidth,600 };
	const Rect rightWind{ 800- sidePanelWidth,0,sidePanelWidth,600};
	double moveTime = 0;

	//プレイヤー弾
	Array<Vec2> playerBullets;
	constexpr double playerShotCoolTime = 0.1;// クールタイム（秒）
	double playerShotTimer = 0.0;// クールタイムタイマー（秒）
	constexpr double playerBulletSpeed = 500.0;// プレイヤーショットのスピード

	while (System::Update())
	{
		const double deltaTime = Scene::DeltaTime();
		moveTime = deltaTime;
		playerShotTimer = Min((playerShotTimer + deltaTime), playerShotCoolTime);

		//プレイヤー移動
		if (KeyA.pressed() && !KeyD.pressed()) {
			pPos.x -= moveTime * pSpeed;
		}
		else if (KeyD.pressed() && !KeyA.pressed()) {
			pPos.x += moveTime * pSpeed;
		}

		if (KeyW.pressed() && !KeyS.pressed()) {
			pPos.y -= moveTime * pSpeed;
		}
		else if (KeyS.pressed() && !KeyW.pressed()) {
			pPos.y += moveTime * pSpeed;
		}
		pPos = { Clamp(pPos.x,0.0 +sidePanelWidth,800.0-sidePanelWidth),Clamp(pPos.y,0.0,600.0) };
		pColi.setPos(Arg::center(pPos));
		//プレイヤー弾発射
		if (playerShotCoolTime <= playerShotTimer)
		{
			playerShotTimer -= playerShotCoolTime;
			playerBullets << pPos.movedBy(0, -50);
		}
		// プレイヤー弾を移動させる
		for (auto& playerBullet : playerBullets)
		{
			playerBullet.y += (deltaTime * -playerBulletSpeed);
		}
		// 画面外に出たプレイヤー弾を削除する
		playerBullets.remove_if([](const Vec2& b) { return (b.y < -40); });

		//--------------描画-------------------------

		pColi.draw(Palette::Black);
		pJetTex.scaled(pJetScale).draw(Arg::center(pPos));
		ground.draw(Palette::Brown);
		leftWind.draw(Palette::Gray);
		rightWind.draw(Palette::Gray);
		houseTex.draw(300, 460);
		// プレイヤー弾を描画する
		for (const auto& playerBullet : playerBullets)
		{
			Circle{ playerBullet, 8 }.draw(Palette::Orange);
		}
	}
}
