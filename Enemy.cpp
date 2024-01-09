#include "Enemy.h"

Enemy::Enemy(double r, double theta)
{
	double spawn_r = StageInfo::earthR + r;
	m_pos = Circular{ spawn_r, theta };
	m_from = m_pos;
	//一番近い家のラジアン 0, π/2, π, 3π/2
	double houseDeg = (static_cast<int>((m_pos.theta + (Math::HalfPi / 2)) / Math::HalfPi) % 4) * Math::HalfPi;
	double enemyRandomTheta = Math::ToRadians(Random(-60, 60)) + houseDeg;
	double enemyRandomR = m_enemyHouseRange + Random(0, 120);

	//OffsetCircularからCircularに変換するためVec2を経由
	m_to = Vec2(OffsetCircular({ Circular(StageInfo::earthR,houseDeg) }, enemyRandomR, enemyRandomTheta));
}

void Enemy::draw() const
{
	if (m_currentHP <= 0)
	{
		m_explosionAnime.drawAt(OffsetCircular({ 0,0 }, m_pos.r, m_pos.theta));
	}
	else
	{
		TextureAsset(U"enemyTex").rotated(m_pos.theta).drawAt(m_collider.center);
	}
}

void Enemy::update()
{
	if (m_currentHP <= 0)
	{
		m_deathFlag = m_explosionAnime.update();
	}
	
	//移動
	const double t = Min(stopwatch.sF() / 10, 1.0);
	m_pos.r = Math::Lerp(m_from.r, m_to.r, t);
	m_pos.theta = Math::LerpAngle(m_from.theta, m_to.theta, t);
	m_collider.setPos(m_pos);
}

bool Enemy::damage(double damage)
{
	m_currentHP -= damage;
	return m_currentHP <= 0;
}

double Enemy::getHP() const
{
	return m_currentHP;
}

bool Enemy::shot(Array<Bullet>& eBulletArr, const Vec2& pJetPos)
{
	//eShotCoolTime経過するごとに発射
	if (stopwatch.sF() > m_eShotCoolTime * (m_shotCnt + 1))
	{
		m_shotCnt++;

		//プレイヤーまでのベクトル
		const Vec2 directPJet = pJetPos - getCenter();
		//街までのベクトル
		const Vec2 directTown = OffsetCircular({ 0,0 }, StageInfo::earthR, ((static_cast<int>((m_pos.theta + (Math::HalfPi / 2)) / Math::HalfPi) % 4) * Math::HalfPi)) - getCollider().center;
		//
		Vec2 direction = {0,0};
		//プレイヤーと街の近いほうを狙う
		if (directTown.lengthSq() < directPJet.lengthSq())
		{
			direction = directTown.normalized();
		}
		else
		{
			direction = directPJet.normalized();
		}
		if (direction.isZero())
		{
			direction = Vec2{ 0,1 };
		}

		eBulletArr << Bullet{ EnemyBullet,0, Circle{Arg::center(getCenter()),eBulletR},direction ,m_bulletDamage };
//----------------------------
		return true;
	}
	return false;
}

Circle Enemy::getCollider() const
{
	return m_collider;
}

bool Enemy::isDeath() const
{
	return m_deathFlag;
}

Circular Enemy::getCenter() const
{
	return m_pos;
}

bool Enemy::isHitThisBullet(int32 bulletID)
{
	for (auto& m_bulletID : m_hitEnhancedBulletArr)
	{
		if (m_bulletID == bulletID)
		{
			return true;
		}
	}
	m_hitEnhancedBulletArr << bulletID;
	return false;
}



