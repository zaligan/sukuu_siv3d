#include "Enemy.h"

Enemy::Enemy(double r, double theta, double earth_r, double enemyHouseRange)
	:earth_r(earth_r)
	, enemyHouseRange(enemyHouseRange)
{
	init(r, theta);
}

void Enemy::draw() const
{
	//collider.draw(Palette::Black);
	TextureAsset(U"enemyTex").rotated(pos.theta).drawAt(collider.center);
}

bool Enemy::calcHP(double damage)
{
	currentHP -= damage;
	deathFlag = currentHP <= 0;
	return deathFlag;
}

void Enemy::shot(Array<Bullet>& eBulletArr, const Vec2& pJetPos)
{
	eShotTimer = fmod(eShotTimer, eShotCoolTime);

	const Vec2 directPJet = pJetPos - getCenter();
	const Vec2 directTown = OffsetCircular({ 0,0 }, earth_r, ((static_cast<int>((pos.theta + (Math::HalfPi/2)) / Math::HalfPi) % 4) * Math::HalfPi)) - getCollider().center;

	if (directTown.lengthSq() < directPJet.lengthSq())
	{
		Vec2 direction = directTown.normalized();

		if (direction.isZero())
		{
			// TODO: ゼロベクトルの場合に飛ばす方向はあとで再検討
			direction = Vec2{ 0,1 };
		}

		eBulletArr << Bullet{ Circle{Arg::center(getCenter()),eBullet_r}, direction };
	}
	else
	{
		Vec2 direction = directPJet.normalized();

		if (direction.isZero())
		{
			// TODO: ゼロベクトルの場合に飛ばす方向はあとで再検討
			direction = Vec2{ 0,1 };
		}

		eBulletArr << Bullet{ Circle{Arg::center(getCenter()),eBullet_r}, directPJet.normalized() };
	}
}

void Enemy::move()
{
	const double t = Min(stopwatch.sF() / 10, 1.0);
	pos.r = Math::Lerp(from.r, to.r, t);
	pos.theta = Math::LerpAngle(from.theta, to.theta, t);
	collider.setPos(OffsetCircular({ 0,0 }, pos.r, pos.theta));
}

Circle Enemy::getCollider()
{
	return collider;
}

bool Enemy::checkDeath()
{
	return deathFlag;
}

double Enemy::geteShotCoolTime()
{
	return eShotCoolTime;
}

void Enemy::init(double r, double theta)
{
	currentHP = maxHP;
	double spawn_r = earth_r + offset_r + r;
	pos = Circular{ spawn_r, theta };
	from = pos;
	//一番近い家のラジアン 0, π/2, π, 3π/2
	double houseDeg = (static_cast<int>((pos.theta + (Math::HalfPi/2)) / Math::HalfPi) % 4) * Math::HalfPi;


	double enemyRandomTheta = Math::ToRadians(Random(-60, 60));
	double enemyRandomR = enemyHouseRange + Random(0, 120);
	//余弦定理してるけどcircularの足し算で行けない？
	double l = sqrt((earth_r * earth_r) + (enemyRandomR * enemyRandomR) - 2 * earth_r * enemyRandomR * Math::Cos(Math::Pi - enemyRandomTheta));
	to = { l,houseDeg + asin((enemyRandomR * sin(Math::Pi - enemyRandomTheta)) / l)};
	if (from.theta - to.theta > Math::Pi)
		to.theta += 2 * Math::Pi;
}

Vec2 Enemy::getCenter() const
{
	return collider.center;
}
