# pragma once
# include "Common.h"

class Enemy
{
public:
	Enemy(Vec2 _r_deg);
	Enemy(double x,double y);
	~Enemy();
	void draw() const;
	bool calcHP(double damage);
	void move(Vec2 to);
	void updatePos();
	void shot(Array<Vec2>& bulletArr);
	Circle getCollider();

private:
	Texture tex{ U"picture/敵/GalagianArtwork/raw/enemies/kamikaze.png" };
	Circle collider{ {0,0},10 };
	Vec2 r_deg{100,100};
	Vec2 old_r_deg{200,200};
	double maxHP = 30;
	double currentHP;
	Stopwatch stopwatch{ StartImmediately::Yes };
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
	const Audio pShotAud{ U"music/se_pyun2.mp3" };
	const Audio eDeathAud{ U"music/maou_se_8bit12.mp3" };

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
	const double pBullet_damage = 10.0;
	double pShotTimer = 0.0;

	//敵
	const Texture enemy1_tex{ U"picture/敵/GalagianArtwork/raw/enemies/kamikaze.png" };
	Array <Enemy> enemy_arr;
	const double eBullet_r = 4.0;
	const double eBullet_speed = 400.0;
	const double eShotCoolTime = 4.0;
	const double eBullet_damage = 10.0;
	double eShotTimer = 0.0;
	const Texture eBullet_tex{ U"picture/敵/GalagianArtwork/raw/projectiles/shotoval.png" };
	Array <Vec2> eBullet_posArr;
	Array <Circle> eBullet_coliArr;

	Rect box = { Arg::center(0,-600),100,30 };


	// 2D カメラ
	// 初期設定: 中心 (0, 0), ズームアップ倍率 1.0
	Camera2D camera{ Vec2{ 0, 0 }, 1.5 };
	Mat3x2 mat = Mat3x2::Identity();
};
