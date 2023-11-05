# pragma once
# include "Common.h"

class Enemy
{
public:
	Enemy(Vec2 _r_deg,Vec2 to);
	~Enemy();
	void draw() const;
	bool calcHP(double damage);
	void move();
	double eShotTimer = 0.0;
	Circle getCollider();
	Vec2 r_deg{ 100,100 };
	double currentHP = 30;
	double animaTime = 0.0;
	Vec2 getFrom();
	Vec2 getTo();

private:
	Texture tex{ U"picture/敵/GalagianArtwork/raw/enemies/kamikaze.png" };
	Circle collider{ {0,0},10 };
	Vec2 from;
	Vec2 to;
	double maxHP = 30;
	Stopwatch stopwatch{ StartImmediately::Yes };
};



// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	bool calcPJetHP(Circle bullet,Circle pJet);

	void update() override;

	void draw() const override;

private:
	const double earth_r = 400.0;
	const Circle earth { 0, 0, earth_r };
	const Texture house{ U"🏠"_emoji };
	const Texture explosion0{ U"picture/敵/GalagianArtwork/raw/flame/flame0.png" };
	const Texture explosion1{ U"picture/敵/GalagianArtwork/raw/flame/flame1.png" };
	const Texture explosion2{ U"picture/敵/GalagianArtwork/raw/flame/flame2.png" };
	const Texture explosion3{ U"picture/敵/GalagianArtwork/raw/flame/flame3.png" };
	const Texture explosion4{ U"picture/敵/GalagianArtwork/raw/flame/flame4.png" };
	const Texture explosion5{ U"picture/敵/GalagianArtwork/raw/flame/flame5.png" };
	const Font font{ FontMethod::SDF,52,Typeface::Bold };
	double deltaTime = 0.0;
	double degrees = 0.0;
	double radians = 0.0;
	int arrNum = 0;
	bool gameOverFlag = false;
	//効果音
	const Audio gameBGM{ U"music/battleBGM.mp3" };
	const Audio pShotAud{ U"music/se_pyun2.mp3" };
	const Audio eDeathAud{ U"music/maou_se_8bit12.mp3" };

	//街
	struct Town
	{
		Circle collider;
		double townHP;
	};
	Array <Town> townArr;
	Circle town0{ Arg::center(0,-earth_r),60};
	Circle town1{ Arg::center(-earth_r,0),60};
	Circle town2{ Arg::center(0,earth_r),60};
	Circle town3{ Arg::center(earth_r,0),60};
	double townHP = 1000.0;

	//プレイヤー
	const Texture pJetTex{ U"picture/敵/GalagianArtwork/raw/player/ship1.png" };
	Circle pJet_collider{ 0,0,10 };
	const double pJet_speed = 0.6;
	const double pJet_r = earth_r + 60;
	Vec2 pJet_r_deg;
	Vec2 pJet_pos{ 0,0 };
	const double pJet_MaxHP = 1.0;
	double pJet_HP;
	const double pBullet_r = 4.0;
	const double pBullet_speed = 400.0;
	const double pBullet_damage = 10.0;
	const double pShotCoolTime = 0.15;
	double pShotTimer = 0.0;

	const Texture shieldTex{ U"picture/敵/GalagianArtwork/raw/player/shield1.png" };
	bool shieldFlag = false;
	Circle shieldCollider{ 0,0,25 };

	const Texture pBullet_tex{ U"picture/敵/GalagianArtwork/raw/projectiles/shotsmall.png" };
	Array <Vec2> pBullet_posArr;
	Array <Circle> pBullet_coliArr;

	//敵
	const Texture enemy1_tex{ U"picture/敵/GalagianArtwork/raw/enemies/kamikaze.png" };
	Array <Enemy> enemy_arr;
	const double eBullet_r = 4.0;
	const double eBullet_speed = 0.4;
	const double eShotCoolTime = 3.0;
	const double eBullet_damage = 10.0;
	const double eSpawnCoolTime = 0.6;
	double eSpawnTimer = 0;
	const Texture eBullet_tex{ U"picture/敵/GalagianArtwork/raw/projectiles/shotoval.png" };
	struct Bullet
	{
		Circle collider;
		Vec2 position;
		Vec2 direction;
	};
	Array <Bullet> eBulletArr;
	Array <Vec2> fromToRandomArr;

	// 2D カメラ
	// 初期設定: 中心 (0, 0), ズームアップ倍率 1.0
	Camera2D camera{ Vec2{ 0, 0 }, 1.5 };
	Mat3x2 mat = Mat3x2::Identity();
};
