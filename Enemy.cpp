#include "Enemy.h"

Enemy::Enemy(double r, double degree, double earth_r, double enemyHouseRange)
	:earth_r(earth_r)
	, enemyHouseRange(enemyHouseRange)
{
	init(r, degree);
}

void Enemy::draw() const
{
	//collider.draw(Palette::Black);
	TextureAsset(U"enemyTex").rotated(r_deg.y).drawAt(collider.center);
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
	const Vec2 directTown = OffsetCircular({ 0,0 }, earth_r, ((static_cast<int>((r_deg.y + (Math::HalfPi/2)) / Math::HalfPi) % 4) * Math::HalfPi)) - getCollider().center;

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
	r_deg = from.lerp(to, t);
	collider.setPos(OffsetCircular({ 0,0 }, r_deg.x, r_deg.y));
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

void Enemy::init(double r, double degree)
{
	currentHP = maxHP;
	r_deg = Vec2{ r, degree } + Vec2{ earth_r + 300, 0 };
	from = r_deg;
	int houseDeg = (static_cast<int>((r_deg.y + (Math::HalfPi/2)) / Math::HalfPi) % 4) * Math::HalfPi;
	double enemyRandomDeg = Random(-60, 60) * Math::Pi / 180;
	double enemyRandomR = enemyHouseRange + Random(0, 120);
	double l = sqrt((earth_r * earth_r) + (enemyRandomR * enemyRandomR) - 2 * earth_r * enemyRandomR * Math::Cos(Math::Pi - enemyRandomDeg));
	to = { l,houseDeg + asin((enemyRandomR * sin(Math::Pi - enemyRandomDeg)) / l)};
	if (from.y - to.y > Math::Pi)
		to.y += 2 * Math::Pi;
}

Vec2 Enemy::getCenter() const
{
	return collider.center;
}
