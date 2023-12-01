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
		HPBar hp;
	};
	int32 townHP = 1000;
	Array <Town> townArr =
	{
		Town{Circle{Arg::center(0,-earth_r),houseSize},HPBar{townHP}},
		Town{Circle{Arg::center(-earth_r,0),houseSize},HPBar{townHP}},
		Town{Circle{Arg::center(0,earth_r),houseSize},HPBar{townHP}},
		Town{Circle{Arg::center(earth_r,0),houseSize},HPBar{townHP}},
	};
	
	//プレイヤー
	const double playerSize = 1.3;
	Circle pJet_collider{ 0,0,playerSize * 10 };
	const double horizSpeed = 0.5;
	const double vertSpeed = 200.0;
	const double maxRotatSpeed = 4.0;//半径bottomの時１周にかかる秒数
	const double minRotatSpeed = 12.0;//半径topの時１周にかかる秒数
	//半径方向のプレイヤーが動ける範囲
	struct MoveRange
	{
		double bottom;
		double top;
	};
	//const MoveRange moveRange{ earth_r + 60,earth_r + 200 };
	const MoveRange moveRange{ 0,earth_r + 200 };
	double pJet_r = moveRange.bottom;
	double radians = 0.0;
	Vec2 pJet_pos{ 0,0 };
	const double pJet_MaxHP = 1.0;
	double pJet_HP;
	const double pBullet_r = 4.0;
	const double pBullet_speed = 400.0;
	const double pBullet_damage = 10.0;
	const double pShotCoolTime = 0.5;
	double pShotTimer = 0.0;
	//アップグレード
	Array <int> pUpgrade = { 0,0,0 };
	double shotSpeedRate = 0.94;//攻撃強化1ごとにかかる倍率
	double shieldUpgRate = 15;//防御強化1ごとに加算する耐久値
	//シールド
	double shieldSize = 1.0;
	bool shieldFlag = false;
	double baseShieldHealth = 200.0;
	double  maxShieldHealth;
	double shieldHealth = baseShieldHealth;
	const double shieldRegenerationRate = 5.0;
	
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
	double spawnTimer = 0;
	double spawnNum = 0;
	const double spawnRate = 1.0;
	struct Item
	{
		int itemType;
		Vec2 r_deg;
	};

	Array <Item> itemArr;

	// 2D カメラ
	const double cameraScale = 2.0;
	Camera2D camera{ Vec2{ 0, 0 }, cameraScale };
	Mat3x2 mat = Mat3x2::Identity();
};
