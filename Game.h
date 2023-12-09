#pragma once
#include"Anime.hpp"
#include"Common.h"
#include"Enemy.h"
#include"HPBar.h"
#include"Player.hpp"
#include"StageInfo.hpp"

// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
	const Circle earth{ 0, 0, earthR };
	const Texture house{ U"🏠"_emoji };
	const Font font{ FontMethod::SDF,52,Typeface::Bold };
	double deltaTime = 0.0;
	double sceneTime = 0.0;

	static constexpr double playerSize = 1.3;
	Player player{ playerSize };
	Array <Bullet> pBulletArr;
	static constexpr double pBullet_speed = 400.0;

	int arrNum = 0;
	bool gameOverFlag = false;
	//効果音
	const Audio gameBGM = AudioAsset(U"gameBGM");
	const Audio pShotAud = AudioAsset(U"pShotAud");
	const Texture backPic = TextureAsset(U"backPic");
	const Texture pJetTex = TextureAsset(U"pJetTex");
	const Texture shieldTex = TextureAsset(U"shieldTex");
	const Texture eBullet_tex = TextureAsset(U"eBullet_tex");
	const Texture enemy1_tex = TextureAsset(U"enemy1_tex");

	//街
	struct Town
	{
		Circle collider;
		HPBar hp;
	};
	int32 townHP = 1000;
	Array <Town> townArr =
	{
		Town{Circle{Arg::center(0,-earthR),houseSize},HPBar{townHP}},
		Town{Circle{Arg::center(-earthR,0),houseSize},HPBar{townHP}},
		Town{Circle{Arg::center(0,earthR),houseSize},HPBar{townHP}},
		Town{Circle{Arg::center(earthR,0),houseSize},HPBar{townHP}},
	};
	

	//アップグレード
	Array <int> pUpgrade = { 0,0,0 };
	double shotSpeedRate = 0.94;//攻撃強化1ごとにかかる倍率
	double shieldUpgRate = 15;//防御強化1ごとに加算する耐久値

	//CSVファイル
	const CSV enemyCSV{ U"csv/EnemyDataSheat.csv" };
	size_t index = 0;

	double itemSpeed = 30.0;

	//Enemy
	Array <Enemy> eArr;
	Array <Bullet> eBulletArr;

	//敵をスポーンさせる時間間隔です
	static constexpr double spawnIntervalSeconds = 1.0;

	//spawnIntervalSecondsごとに出現させる敵の数です
	static constexpr int32 spawnCnt = 1;

	//敵のスポーン時間を管理します
	double eSpawnTimer = 0;

	//敵が出現する地表からの距離範囲です
	static constexpr double maxSpawnR = 330;
	static constexpr double minSpawnR = 310;

	//敵が出現する角度成分(度数法)です
	static constexpr double maxSpawnTheta = 360;
	static constexpr double minSpawnTheta = 0;


	const double eBullet_speed = 100.0;
	const double eBullet_damage = 10.0;

	
	struct Item
	{
		int itemType;
		Circular pos;
	};

	Array <Item> itemArr;

	// 2D カメラ
	const double cameraScale = 2.0;
	static constexpr bool cameraMode = true;
	static constexpr double cameraOffsetY = 90;

	Camera2D camera{ Vec2{ 0, 0 }, cameraScale,CameraControl::None_ };

	Mat3x2 mat = Mat3x2::Identity();
};
