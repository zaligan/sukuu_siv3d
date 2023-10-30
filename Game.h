# pragma once
# include "Common.h"

class Enemy
{
public:
	Enemy(Texture tex,Circle collider);
	~Enemy();
	void draw() const;

private:
	Texture tex;
	Circle collider;
	Vec2 deg_r;
	const double maxHP = 100;
	double currentHP;
};

// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
	const Texture house{ U"🏠"_emoji };
	const double earth_r = 400.0;
	double deltaTime = 0.0;
	double degrees = 0.0;
	double radians = 0.0;
	int arrNum = 0;
	//効果音
	const Audio pShotAud{ U"music/発射2.mp3" };

	//プレイヤー
	const Texture pJetTex{ U"picture/敵/GalagianArtwork/raw/player/ship1.png" };
	const double pJet_speed = 0.6;
	const double pJet_r = earth_r + 60;
	Vec2 pJet_pos{ 0,0 };

	const Texture pBullet_tex{ U"picture/敵/GalagianArtwork/raw/projectiles/shotsmall.png" };
	Array <Vec2> pBullet_posArr;
	Array <Circle> pBullet_coliArr;
	//test
	const double pBullet_r = 4.0;
	const double pBullet_speed = 400.0;
	const double pShotCoolTime = 0.1;
	double pShotTimer = 0.0;

	//敵
	const Texture enemy1_tex{ U"picture/敵/GalagianArtwork/raw/enemies/kamikaze.png" };
	Array <Circle> enemy1_coliArr;
	Circle n1 { 0, -earth_r - 170,10 };
	Circle n2 { 30, -earth_r - 160,10 };
	Circle n3 { -50, -earth_r - 180,10 };
	Enemy eee{ enemy1_tex,n1 };

	const double enemy1_speed = 1.0;

	const Texture eBullet_tex{ U"picture/敵/GalagianArtwork/raw/projectiles/shotoval.png" };

	Rect box = { Arg::center(0,-600),100,30 };


	// 2D カメラ
	// 初期設定: 中心 (0, 0), ズームアップ倍率 1.0
	Camera2D camera{ Vec2{ 0, 0 }, 1.5 };
	Mat3x2 mat = Mat3x2::Identity();
};
