# pragma once
# include "Common.h"

// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
	const Texture cat{ U"🐈"_emoji };

	// 2D カメラ
	// 初期設定: 中心 (0, 0), ズームアップ倍率 1.0
	Camera2D camera{ Vec2{ 0, 0 }, 1.0 };


	//const Texture houseTex{ U"🏚️"_emoji };
	const Texture pJetTex{ U"picture/敵/GalagianArtwork/raw/enemies/fighter.png" };
	//const double pJetScale = 0.02;
	//const double pSpeed = 300.0;//プレイヤーの移動速度
	//const int sidePanelWidth = 200.0;//両脇パネルの横幅
	//Vec2 pPos{ 300,300 };
	//Circle pColi{ 0,0,33 };

	//const Rect ground{ 0,550,800,50 };
	//const Rect leftWind{ 0,0,sidePanelWidth,600 };
	//const Rect rightWind{ 800 - sidePanelWidth,0,sidePanelWidth,600 };
	//double moveTime = 0;

	////プレイヤー弾
	//Array<Vec2> playerBullets;
	//const double playerShotCoolTime = 0.1;// クールタイム（秒）
	//double playerShotTimer = 0.0;// クールタイムタイマー（秒）
	//const double playerBulletSpeed = 500.0;// プレイヤーショットのスピード


};
