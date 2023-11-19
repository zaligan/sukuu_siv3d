#include "Enemy.h"

Enemy::Enemy(ReadEnemyData enemyData, double earth_r, double enemyHouseRange)
	:enemyData(enemyData)
	,earth_r(earth_r)
	,enemyHouseRange(enemyHouseRange)
{
	eShotCoolTime = Random(2.7, 3.0);
	r_deg = { enemyData.r + earth_r+300,enemyData.deg};
	from = r_deg;
	int houseDeg = (static_cast<int>((r_deg.y + 45.0) / 90) % 4) * 90;
	double enemyRandomDeg = Random(-60, 60) * Math::Pi /180;
	double enemyRandomR = enemyHouseRange + Random(0,120);
	double l = sqrt((earth_r * earth_r) + (enemyRandomR * enemyRandomR) - 2 * earth_r * enemyRandomR * Math::Cos(Math::Pi - enemyRandomDeg));
	to = { l,houseDeg + asin((enemyRandomR * sin(Math::Pi - enemyRandomDeg)) / l) * 180 /Math::Pi };
}

Enemy::~Enemy()
{
}

void Enemy::draw() const
{
	//collider.draw(Palette::Black);
	tex.rotated(r_deg.y*Math::Pi/180).drawAt(collider.center);
}

bool Enemy::calcHP(double damage)
{
	currentHP -= damage;
	if (currentHP <= 0)
		return true;
	else
		return false;
}

void Enemy::Shot(Array<Bullet>& eBulletArr,Vec2 pJetPos)
{
	eShotTimer = fmod(eShotTimer, eShotCoolTime);
	Vec2 directPJet = pJetPos - getCollider().center;
	Vec2 directTown = OffsetCircular({ 0,0 }, earth_r, ((static_cast<int>((r_deg.y + 45.0) / 90) % 4) * 90)*Math::Pi/180) - getCollider().center;
	if(directPJet.x * directPJet.x + directPJet.y * directPJet.y > directTown.x * directTown.x + directTown.y * directTown.y)
		eBulletArr << Bullet{ Circle{0,0,eBullet_r},Vec2{getCollider().center},directTown };
	else
		eBulletArr << Bullet{ Circle{0,0,eBullet_r},Vec2{getCollider().center},directPJet };
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

ReadEnemyData Enemy::getData()
{
	return enemyData;
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
