# pragma once
# include <Siv3D.hpp>

struct Bullet
{
	Circle collider;

	// 単位ベクトル
	Vec2 direction;
};

constexpr double eBullet_r = 4.0;
