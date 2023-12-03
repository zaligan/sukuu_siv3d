#include "Enemy.h"

Enemy::Enemy(double r, double degree, double earth_r, double enemyHouseRange)
	:earth_r(earth_r)
	,enemyHouseRange(enemyHouseRange)
{
	init(r, degree);
}

void Enemy::draw() const
{
	//collider.draw(Palette::Black);
	TextureAsset(U"enemyTex").rotated(r_deg.y*Math::Pi/180).drawAt(collider.center);
}

bool Enemy::calcHP(double damage)
{
	currentHP -= damage;
	deathFlag = currentHP <= 0;
	return deathFlag;
}

void Enemy::Shot(Array<Bullet>& eBulletArr, const Vec2& pJetPos)
{
	eShotTimer = fmod(eShotTimer, eShotCoolTime);

	const Vec2 directPJet = pJetPos - getCenter();
	const Vec2 directTown = OffsetCircular({ 0,0 }, earth_r, ((static_cast<int>((r_deg.y + 45.0) / 90) % 4) * 90)*Math::Pi/180) - getCollider().center;

	if (directTown.lengthSq() < directPJet.lengthSq())
	{
		eBulletArr << Bullet{ Circle{Arg::center(getCenter()),eBullet_r},directTown };
	}
	else
	{
		eBulletArr << Bullet{ Circle{Arg::center(getCenter()),eBullet_r},directPJet };
	}
}

void Enemy::move()
{
	const double t = Min(stopwatch.sF() / 10, 1.0);
	r_deg = from.lerp(to, t);
	collider.setPos(OffsetCircular({0,0}, r_deg.x,r_deg.y*Math::Pi/180));
}

Circle Enemy::getCollider()
{
	return collider;
}

bool Enemy::getDeathFlag()
{
	return deathFlag;
}

double Enemy::geteShotCoolTime()
{
	return eShotCoolTime;
}

Vec2 Enemy::getFrom()
{
	return from;
}

Vec2 Enemy::getTo()
{
	return to;
}

Vec2 Enemy::getPos()
{
	return OffsetCircular({ 0,0 }, r_deg.x, r_deg.y * Math::Pi / 180);
}

void Enemy::init(double r, double degree)
{
	currentHP = maxHP;
	eShotCoolTime = Random(2.7, 3.0);
	r_deg = Vec2{ r, degree } + Vec2{ earth_r + 300, 0 };
	from = r_deg;
	int houseDeg = (static_cast<int>((r_deg.y + 45.0) / 90) % 4) * 90;
	double enemyRandomDeg = Random(-60, 60) * Math::Pi / 180;
	double enemyRandomR = enemyHouseRange + Random(0, 120);
	double l = sqrt((earth_r * earth_r) + (enemyRandomR * enemyRandomR) - 2 * earth_r * enemyRandomR * Math::Cos(Math::Pi - enemyRandomDeg));
	to = { l,houseDeg + asin((enemyRandomR * sin(Math::Pi - enemyRandomDeg)) / l) * 180 / Math::Pi };
	if (from.y - to.y > 180.0)
		to.y += 360.0;
}

Vec2 Enemy::getCenter() const
{
	return collider.center;
}
