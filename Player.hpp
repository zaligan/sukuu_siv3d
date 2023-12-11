#pragma once
#include <Siv3D.hpp>
#include "Anime.hpp"
#include "Bullet.hpp"
#include "StageInfo.hpp"

//移動方向を表す列挙型
enum class Directions
{
	Up,
	Down,
	Left,
	Right
};

class Player
{
public:

	Player();
	Player(double playerSize):m_playerSize(playerSize){};

	void update(double deltaTime)
	{
		m_deltaTime = deltaTime;
		m_shotTimer += m_deltaTime;
		m_collider.setCenter(m_pos);
		//自然回復
		m_shieldCurrentHP = std::min(m_maxShieldHP, m_shieldCurrentHP + m_shieldRegenerationRate * deltaTime);
	}

	void move( Directions directions)
	{
		switch (directions)
		{
		case Directions::Up:
			m_pos.r += m_vertSpeed* m_deltaTime;
			break;

		case Directions::Down:
			m_pos.r -= m_vertSpeed * m_deltaTime;
			break;

		case Directions::Left:
			m_pos.theta -= 2 * Math::Pi * m_deltaTime / (m_maxRotatSpeed + ((m_minRotatSpeed - m_maxRotatSpeed) * ((getR() - m_moveRange.minRadius) / (m_moveRange.maxRadius - m_moveRange.minRadius))));
			break;

		case Directions::Right:
			m_pos.theta += 2 * Math::Pi * m_deltaTime / (m_maxRotatSpeed + ((m_minRotatSpeed - m_maxRotatSpeed) * ((getR() - m_moveRange.minRadius) / (m_moveRange.maxRadius - m_moveRange.minRadius))));
			break;
		}
		m_pos.r = Clamp(m_pos.r, m_moveRange.minRadius, m_moveRange.maxRadius);
	}

	void draw() const
	{
		if (m_shieldFlag)
		{
			m_shieldAnime.drawAt(m_pos, m_pos.r);
		}
	}

	void shot(Array< Bullet >& bulletArr)
	{
		if (m_shotTimer > m_shotCoolTime)
		{
			m_shotTimer = fmod(m_shotTimer, m_shotCoolTime);

			Vec2 direction = Vec2(Circular{1,getTheta() }).normalized();

			bulletArr << Bullet{ Circle{getCenter(),pBulletR}, direction };

			AudioAsset(U"pShotAud").playOneShot();
		}
	}

	double getHP() const
	{
		return m_currentHP;
	}

	void damage(double damege)
	{
		m_currentHP -= damege;
	}

	Vec2 getCenter() const
	{
		return Vec2( m_pos );
	}

	Circular getCircular() const
	{
		return m_pos;
	}

	double getR() const
	{
		return m_pos.r;
	}

	double getTheta() const
	{
		return m_pos.theta;
	}

	Circle getCollider() const
	{
		return m_collider;
	}
	//シールド関連

	void useShield(bool flag)
	{
		//flagが true -> false に変わったら
		if (m_shieldFlag && !flag)
		{
			m_shieldAnime.reset();
		}

		m_shieldFlag = flag;
	}

	bool isShieldAvailable() const
	{
		return m_shieldFlag && m_shieldCurrentHP > 0;
	}

	bool shieldUpdate()
	{
		m_shieldCollider.setCenter(m_pos);
		return m_shieldAnime.update();
	}

	Circle getShieldCollider()
	{
		return m_shieldCollider;
	}

	void shieldDamage(double damage)
	{
		m_shieldCurrentHP -= damage;
	}

	void shieldDraw(Circular pos) const
	{
		m_shieldCollider.draw(Palette::Dodgerblue);
		m_shieldAnime.drawAt(Circular(pos.r + m_shieldAnimePosOffset.r, pos.theta + m_shieldAnimePosOffset.theta), pos.theta);
	}

	void shieldRestoreHP(double heal)
	{
		m_shieldCurrentHP += heal;
	}
	
private:
	
	double m_deltaTime = 0;
	Circular m_pos{ 0,0 };
	double m_playerSize = 1.0;
	Circle m_collider{ 0,0,m_playerSize * 10 };
	static constexpr double m_horizSpeed = 0.5;
	static constexpr double m_vertSpeed = 200.0;
	static constexpr double m_maxRotatSpeed = 6.0;//半径bottomの時１周にかかる秒数
	static constexpr double m_minRotatSpeed = 18.0;//半径topの時１周にかかる秒数
	//半径方向のプレイヤーが動ける範囲
	struct MoveRange
	{
		double minRadius;
		double maxRadius;
	};
	static constexpr MoveRange m_moveRange{ 100,earthR + 200 };
	double radians = 0.0;
	static constexpr double m_maxHP = 1.0;
	double m_currentHP = m_maxHP;
	
	
	
	static constexpr double m_shotCoolTime = 0.5;
	double m_shotTimer = 0.0;

	//シールド
	double m_shieldSize = 1.0;
	bool m_shieldFlag = false;
	double m_baseShieldHP = 200.0;
	double  m_maxShieldHP = 200.0;
	double m_shieldCurrentHP = m_baseShieldHP;
	static constexpr double m_shieldRegenerationRate = 5.0;
	Anime m_shieldAnime{ TextureAsset(U"shieldTex"), 4, 5, 0.04, 0.18 };
	Circular m_shieldAnimePosOffset{ 3,0 };
	Circle m_shieldCollider{ m_shieldSize * 30.0 };

};
