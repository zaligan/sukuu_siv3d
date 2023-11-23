# pragma once
#include"Anime.h"
#include"Common.h"
#include"Enemy.h"
#include"HPBar.h"
// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	bool calcPJetHP(Circle bullet,Circle pJet);

	void update() override;

	void draw() const override;

private:
	const double earth_r = getData().earth_r;
	const double houseSize = getData().houseSize;
	const Circle earth { 0, 0, earth_r };
	const Texture house{ U"🏠"_emoji };
	const Font font{ FontMethod::SDF,52,Typeface::Bold };
	double deltaTime = 0.0;
	double sceneTime = 0.0;
	
	int arrNum = 0;
	bool gameOverFlag = false;
	//効果音
	const Audio gameBGM = AudioAsset(U"gameBGM");
	const Audio pShotAud = AudioAsset(U"pShotAud");
	const Texture backPic = TextureAsset(U"backPic");
	const Texture pJetTex = TextureAsset(U"pJetTex");
	const Texture shieldTex = TextureAsset(U"shieldTex");
	const Texture pBullet_tex = TextureAsset(U"pBullet_tex");
	const Texture eBullet_tex = TextureAsset(U"eBullet_tex");
	const Texture enemy1_tex = TextureAsset(U"enemy1_tex");

	//街
	struct Town
	{
		Circle collider;
		int32 townHP;
	};
	Array <Town> townArr;
	Circle town0{ Arg::center(0,-earth_r),houseSize};
	Circle town1{ Arg::center(-earth_r,0),houseSize };
	Circle town2{ Arg::center(0,earth_r),houseSize };
	Circle town3{ Arg::center(earth_r,0),houseSize };
	int32 townHP = 1000;

	//プレイヤー
	Array <int> pUpgrade = {0,0,0};
	Circle pJet_collider{ 0,0,10 };
	const double pJet_speed = 0.6;
	const double pJet_r = earth_r + 60;
	double radians = 0.0;
	Vec2 pJet_r_deg;
	Vec2 pJet_pos{ 0,0 };
	const double pJet_MaxHP = 1.0;
	double pJet_HP;
	const double pBullet_r = 4.0;
	const double pBullet_speed = 400.0;
	const double pBullet_damage = 10.0;
	const double pShotCoolTime = 0.15;
	double pShotTimer = 0.0;

	
	bool shieldFlag = false;
	Circle shieldCollider{ 0,0,25 };
	
	Array <Vec2> pBullet_posArr;
	Array <Circle> pBullet_coliArr;

	//CSVファイル
	const CSV enemyCSV{ U"csv/EnemyDataSheat.csv" };
	Array<ReadEnemyData> readEnemyDataArr;
	const size_t enemyCount = enemyCSV.rows();
	size_t addLine = 0;

	double itemSpeed = 30.0;

	//Enemy
	Array <Enemy> enemy_arr;
	const double eBullet_speed = 0.4;
	const double eBullet_damage = 10.0;
	const double eSpawnCoolTime = 0.1;
	double eSpawnTimer = 0;
	Array <Bullet> eBulletArr;
	Array <Vec2> fromToRandomArr;

	struct Item
	{
		int itemType;
		Vec2 r_deg;
	};

	Array <Item> itemArr;

	// 2D カメラ
	// 初期設定: 中心 (0, 0), ズームアップ倍率 1.0
	Camera2D camera{ Vec2{ 0, 0 }, 1.5 };
	Mat3x2 mat = Mat3x2::Identity();

	//HPBar
	Array<HPBar> hpBars =
	{
		HPBar{ townHP },HPBar{ townHP },HPBar{ townHP },HPBar{ townHP }
	};
};
