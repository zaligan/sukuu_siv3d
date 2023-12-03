
# pragma once
# include <Siv3D.hpp>

struct Bullet
{
	Circle collider;

	// 単位ベクトル
	Vec2 direction;
};

