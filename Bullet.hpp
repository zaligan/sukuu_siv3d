# pragma once
# include <Siv3D.hpp>

//弾の種類を表します
enum BulletType
{
	//プレイヤーの普通の弾
	Normal,

	//強化時のプレイヤーの弾
	Enhanced,

	//敵の弾
	EnemyBullet,

	//街の弾
	TownBullet
};

//敵とプレイヤーの弾のクラスです
struct Bullet
{
	//弾の種類です
	BulletType type;

	//弾の個体識別IDです
	int32 ID;

	//衝突範囲です
	Circle collider;

	// 進行方向を表す単位ベクトルです
	Vec2 direction;

	//この弾のダメージです
	double damage;
};

/// @brief プレイヤーの弾速
static constexpr double pBullet_speed = 400.0;
constexpr double pBulletR = 6.0;
constexpr double pEnhancedBulletR = 9.0;

constexpr double eBulletR = 4.0;
constexpr double eBulletSpeed = 40.0;

