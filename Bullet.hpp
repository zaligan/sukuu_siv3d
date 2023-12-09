# pragma once
# include <Siv3D.hpp>

struct Bullet
{
	Circle collider;

	// 単位ベクトル
	Vec2 direction;
};

constexpr double pBulletDamage = 10.0;
constexpr double pBulletR = 4.0;


constexpr double eBulletR = 4.0;
