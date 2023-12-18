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
	/// @brief プレイヤーを作成します
	/// @param playerSize プレイヤーのサイズを指定します
	Player(double playerSize):m_playerSize(playerSize){};

	/// @brief フレーム時間に合わせて更新します
	/// @param deltaTime フレーム時間を渡します
	void update(double deltaTime)
	{
		m_deltaTime = deltaTime;
		m_shotTimer += m_deltaTime;
		m_collider.setCenter(m_pos);
		//自然回復
		m_shieldCurrentHP = std::min(m_maxShieldHP, m_shieldCurrentHP + m_shieldRegenerationRate * deltaTime);
	}

	/// @brief 入力に応じてプレイヤーを移動します
	/// @param directions 入力されている方向を表すパラメータです
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

	/// @brief プレイヤーを描画します
	void draw() const
	{
		if (m_shieldFlag)
		{
			m_shieldAnime.drawAt(m_pos, m_pos.r);
		}
	}

	/// @brief 弾を発射します
	/// @param bulletArr プレイヤーの弾を管理する配列です
	void shot(Array< Bullet >& bulletArr)
	{
		if(m_enhancedMode)
		{
			//強化時の射撃
			if (m_shotTimer > m_enhancedShotCoolTime)
			{
				m_shotTimer = 0;

				Vec2 direction = Vec2(Circular{ 1,getTheta() }).normalized();

				bulletArr << Bullet{ Enhanced,m_enhancedBulletID,Circle{getCenter(),pEnhancedBulletR}, direction };

				m_enhancedBulletID++;

				AudioAsset(U"pShotAud").playOneShot();
			}
		}
		else
		{
			//通常時の射撃
			if (m_shotTimer > m_shotCoolTime)
			{
				m_shotTimer = 0;

				Vec2 direction = Vec2(Circular{ 1,getTheta() }).normalized();

				bulletArr << Bullet{ Normal,0,Circle{getCenter(),pBulletR}, direction };

				AudioAsset(U"pShotAud").playOneShot();
			}
		}
	}

	/// @brief 現在のHPを返します
	/// @return 現在のHPです
	double getHP() const
	{
		return m_currentHP;
	}

	/// @brief プレイヤーにダメージを与えます
	/// @param damege 受けるダメージ量です
	void damage(double damege)
	{
		m_currentHP -= damege;
	}

	/// @brief プレイヤーの中心座標をVec2で返します
	/// @return Vec2型のプレイヤー中心座標
	Vec2 getCenter() const
	{
		return Vec2( m_pos );
	}

	/// @brief プレイヤーの中心座標をCircularで返します
	/// @return Circular型のプレイヤー中心座標
	Circular getCircular() const
	{
		return m_pos;
	}

	/// @brief マップ中央からプレイヤーの距離を返します
	/// @return マップ中央からプレイヤーの距離
	double getR() const
	{
		return m_pos.r;
	}

	/// @brief プレイヤーの回転角度を返します
	/// @return プレイヤーの回転角度
	double getTheta() const
	{
		return m_pos.theta;
	}

	/// @brief プレイヤーの衝突範囲を返します
	/// @return プレイヤーの衝突範囲
	Circle getCollider() const
	{
		return m_collider;
	}

	/// @brief シールドを展開します
	/// @param flag trueの時シールドを展開します
	void useShield(bool isShieldUsed)
	{
		//flagが true -> false に変わったらアニメーションをリセット
		if (m_shieldFlag && !isShieldUsed)
		{
			m_shieldAnime.reset();
		}

		m_shieldFlag = isShieldUsed;
	}

	/// @brief シールドが使用可能かチェックします
	/// @return シールドが使用できる時true
	bool isShieldAvailable() const
	{
		return m_shieldFlag && m_shieldCurrentHP > 0;
	}

	/// @brief シールドを更新します
	void shieldUpdate()
	{
		m_shieldCollider.setCenter(m_pos);
		m_shieldAnime.update();
	}

	/// @brief シールドの衝突範囲を返します
	/// @return シールドの衝突範囲
	Circle getShieldCollider()
	{
		return m_shieldCollider;
	}

	/// @brief シールドにダメージを与えます
	/// @param damage 受けるダメージ量です
	void shieldDamage(double damage)
	{
		m_shieldCurrentHP -= damage;
	}

	/// @brief シールドを描画します
	/// @param pos 描画する位置です
	void shieldDraw(Circular pos) const
	{
		m_shieldAnime.drawAt(Circular(pos.r + m_shieldAnimePosOffset.r, pos.theta + m_shieldAnimePosOffset.theta), pos.theta);
	}

	/// @brief シールドを回復します
	/// @param heal 回復量です
	void shieldRestoreHP(double heal)
	{
		m_shieldCurrentHP += heal;
	}

	void enhancedPlayer(bool isEnhanced)
	{
		m_enhancedMode = isEnhanced;
	}
	
private:
	//１フレームの時間です
	double m_deltaTime = 0;

	//プレイヤーの中心座標(半径、回転角)です
	Circular m_pos{ earthR,0 };

	//プレイヤーのサイズです
	double m_playerSize = 1.0;

	//プレイヤーの衝突範囲です
	Circle m_collider{ 0,0,m_playerSize * 10 };

	//上下方向のプレイヤーが動ける範囲
	struct MoveRange
	{
		double minRadius;
		double maxRadius;
	};
	static constexpr MoveRange m_moveRange{ 100,earthR + 200 };

	//上下方向の移動速度です
	static constexpr double m_vertSpeed = 200.0;

	//左右方向の移動速度です,MoveRangeによって変化
	//プレイヤーがminRadiusにいる時１周にかかる秒数
	static constexpr double m_maxRotatSpeed = 6.0;
	//プレイヤーがmaxRadiusにいる時１周にかかる秒数
	static constexpr double m_minRotatSpeed = 18.0;

	//プレイヤーの最大体力です
	static constexpr double m_maxHP = 1.0;

	//プレイヤーの現在の体力です
	double m_currentHP = m_maxHP;

	//射撃間隔の時間(秒)です
	static constexpr double m_shotCoolTime = 0.5;

	//射撃してからの時間を計ります
	double m_shotTimer = 0.0;

	//射撃の強化モードです
	bool m_enhancedMode = false;

	//強化モード時の射撃間隔(秒)です
	double m_enhancedShotCoolTime = 0.15;

	//強化した弾のIDです
	int32 m_enhancedBulletID = 0;

//-------シールド-----------

	//シールドのサイズです
	double m_shieldSize = 1.0;

	//シールドの使用状況です
	bool m_shieldFlag = false;

	//シールドの最大耐久値です
	double  m_maxShieldHP = 200.0;

	//シールドの現在の耐久値です
	double m_shieldCurrentHP = m_maxShieldHP;

	//毎秒自動回復する量です
	static constexpr double m_shieldRegenerationRate = 5.0;

	//シールド使用時のアニメーションです
	Anime m_shieldAnime{Point(4,1), TextureAsset(U"shieldTex"), 3, 5, 0.04, 0.5 };

	//シールドの衝突範囲です
	Circle m_shieldCollider{ m_shieldSize * 30.0 };

	//アニメーションの位置と衝突範囲の調整用
	Circular m_shieldAnimePosOffset{ 0,0 };
	
};
