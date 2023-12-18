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
	EnemyBullet
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
};

constexpr double pBulletR = 4.0;
constexpr double pBulletDamage = 10.0;
constexpr double pEnhancedBulletR = 8.0;
constexpr double pEnhancedBulletDamage = 20.0;



constexpr double eBulletR = 4.0;
constexpr double eBulletSpeed = 100.0;
constexpr double eBulletDamage = 10.0;
