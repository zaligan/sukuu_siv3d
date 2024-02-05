#pragma once
#include <Siv3D.hpp>
#include "Anime.hpp"
#include "Bullet.hpp"
#include "StageInfo.hpp"
#include "Upgrade.hpp"

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
		shieldUpdate();
		m_collider.setCenter(m_pos);
		//自然回復
		if (!m_shieldFlag)
		{
			m_shieldCurrentHP = std::min(m_maxShieldHP, m_shieldCurrentHP + m_shieldRegenerationRate * deltaTime);
		}

		//強化値が一定を超えたら強化モード
		if (m_enhancePoint > m_enhanceThreshold)
		{
			//強化時１度だけ鳴らす
			if (!m_enhancedMode)
			{
				AudioAsset(U"playerEnhancedAud").playOneShot();
			}
			m_enhancedMode = true;
		}

		//強化値が一定を超えたらシールド全回復
		if (m_enhancePoint > m_shieldRestoreThreshold * (m_shieldRestoreCnt + 1))
		{
			m_shieldRestoreCnt++;
			shieldRestoreHP(m_maxShieldHP - m_shieldCurrentHP);
		}

		//強化値が0以下の時強化モード解除
		if (m_enhancePoint <= 0)
		{
			m_enhancedMode = false;
			m_shieldRestoreCnt = 0;
			m_enhanceEffectAnime.reset();
		}

		m_enhanceEffectAnime.update();
		
	}

	/// @brief 入力に応じてプレイヤーを移動します
	/// @param directions 入力されている方向を表すパラメータです
	void move( double moveInput_X,double moveInput_Y)
	{
		m_pos.r += m_vertSpeed * m_deltaTime * moveInput_Y;
		m_pos.theta += 2 * Math::Pi * m_deltaTime / (m_maxRotateSpeed + ((m_minRotateSpeed - m_maxRotateSpeed) * ((getR() - m_moveRange.minRadius) / (m_moveRange.maxRadius - m_moveRange.minRadius)))) * moveInput_X;
		m_pos.r = Clamp(m_pos.r, m_moveRange.minRadius, m_moveRange.maxRadius);
	}

	void move(Vec2 moveInput)
	{
		move(moveInput.x, moveInput.y);
	}

	/// @brief プレイヤーを描画します
	void draw() const
	{
		TextureAsset(U"pJetTex").scaled(m_playerSize).rotated(m_pos.theta).drawAt(m_pos);
		if (m_shieldFlag)
		{
			shieldDraw(m_pos);
		}
		if (m_enhancedMode)
		{
			m_enhanceEffectAnime.drawAt(m_pos, m_pos.theta);
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

				bulletArr << Bullet{ Enhanced,m_enhancedBulletID,Circle{getCenter(),pEnhancedBulletR}, direction ,m_enhancedBulletDamage };

				m_enhancedBulletID++;

				m_enhancePoint = Max(m_enhancePoint- m_shotLostEnhancePoint,0.0);

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

				bulletArr << Bullet{ Normal,0,Circle{getCenter(),pBulletR}, direction ,m_bulletDamage };

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
		//シールドを解除したら
		if (m_shieldFlag && !isShieldUsed)
		{
			m_shieldAnime.reset();

			//非強化状態でシールドを解いたら強化ポイントリセット
			if (!m_enhancedMode)
			{
				m_enhancePoint = 0;
			}
		}

		m_shieldFlag = isShieldUsed;
	}

	/// @brief シールドが使用可能かチェックします
	/// @return シールドが使用できる時true
	bool isShieldAvailable() const
	{
		return m_shieldFlag && m_shieldCurrentHP > 0;
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

	/// @brief シールドを回復します
	/// @param heal 回復量です
	void shieldRestoreHP(double heal)
	{
		m_shieldCurrentHP += heal;
		AudioAsset(U"shieldRestoreAud").playOneShot();
	}

	void addEnhancePoint(double addPoint)
	{
		m_enhancePoint += addPoint;
	}

	/// @brief アップグレードアイテムの所持数を加算
	/// @param itemType アップグレードの種類
	void addOnePointUpgrade(int32 itemType)
	{
		switch (itemType)
		{
		case 0:
			m_upgrade.Attack++;
			break;
		case 1:
			m_upgrade.Defense++;
			break;
		case 2:
			m_upgrade.Special++;
			break;
		default:
			break;
		}
	}

	/// @brief 所持アップグレード数をすべて0にする
	void resetUpgrade()
	{
		m_upgrade = { 0,0,0 };
	}

	/// @brief 所持アップグレード数を返します
	/// @return 所持アップグレード数
	Upgrade getUpgradeCnt() const
	{
		return m_upgrade;
	}


private:

	/// @brief シールドを更新します
	void shieldUpdate()
	{
		m_shieldCollider.setCenter(m_pos);
		m_shieldAnime.update();
	}

	/// @brief シールドを描画します
	/// @param pos 描画する位置です
	void shieldDraw(Circular pos) const
	{
		double colorH = (m_maxShieldHP - m_shieldCurrentHP) / m_maxShieldHP * 110;
		m_shieldCollider.draw(ColorF(HSV{ 250 + colorH,0.9,1 }, 0.7));
		m_shieldAnime.drawAt(Circular(pos.r + m_shieldAnimePosOffset.r, pos.theta + m_shieldAnimePosOffset.theta), pos.theta);
	}

	//１フレームの時間です
	double m_deltaTime = 0;

	//プレイヤーの中心座標(半径、回転角)です
	Circular m_pos{ StageInfo::earthR,0 };

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
	static constexpr MoveRange m_moveRange{ 100,StageInfo::earthR + 200 };

	//上下方向の移動速度です
	static constexpr double m_vertSpeed = 200.0;

	//左右方向の移動速度です,MoveRangeによって変化
	//プレイヤーが,minRadiusにいる時１周にかかる秒数
	static constexpr double m_maxRotateSpeed = 3.0;
	//プレイヤーが,maxRadiusにいる時１周にかかる秒数
	static constexpr double m_minRotateSpeed = 18.0;

	//プレイヤーの最大体力です
	static constexpr double m_maxHP = 1.0;

	//プレイヤーの現在の体力です
	double m_currentHP = m_maxHP;

	//射撃間隔の時間(秒)です
	static constexpr double m_shotCoolTime = 0.5;

	//射撃してからの時間を計ります
	double m_shotTimer = 0.0;

//-------強化状態--------------------------

	//強化状態のとき,true
	bool m_enhancedMode = false;

	//シールドで吸収した強化値です
	double m_enhancePoint = 0;

	//m_enhancePointがこの値を超えると強化状態
	double m_enhanceThreshold = 30;

	//m_enhancePointがこの値を超えるとシールド全回復
	double m_shieldRestoreThreshold = 50;

	//m_enhancePointが,shieldRestoreThresholdを超えた回数
	int32 m_shieldRestoreCnt = 0;

	//強化状態で発射した時に減るm_enhancePointの量
	static constexpr double m_shotLostEnhancePoint = 10.0;

	//プレイヤー強化時のエフェクトです
	Anime m_enhanceEffectAnime{ Point(4,1), TextureAsset(U"enhancedEffectTex"), 3, 5, 0.04, 0.5 };

	//強化モード時の射撃間隔(秒)です
	double m_enhancedShotCoolTime = 0.15;

	//アップグレードアイテム
	Upgrade m_upgrade;
//-------弾--------------------

	//通常時の弾の威力
	static constexpr double m_bulletDamage = 10.0;

	//強化時の弾の威力
	static constexpr double m_enhancedBulletDamage = 20.0;

	//強化した弾のIDです
	int32 m_enhancedBulletID = 0;

//-------シールド-----------

	//シールドのサイズです
	double m_shieldSize = 1.0;

	//シールドの使用状況です
	bool m_shieldFlag = false;

	//シールドの最大耐久値です
	double  m_maxShieldHP = 1000.0;

	//シールドの現在の耐久値です
	double m_shieldCurrentHP = m_maxShieldHP;

	//毎秒自動回復する量です
	static constexpr double m_shieldRegenerationRate = 100.0;

	//シールド使用時のアニメーションです
	Anime m_shieldAnime{TextureAsset(U"shieldTex"), 4, 5, 0.03, m_shieldSize * 0.18 };

	//シールドの衝突範囲です
	Circle m_shieldCollider{ m_shieldSize * 30.0 };

	//アニメーションの位置と衝突範囲の調整用
	Circular m_shieldAnimePosOffset{ 3,0 };
	
};
