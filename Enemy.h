#pragma once
#include <Siv3D.hpp>
#include "Anime.hpp" 
#include "Bullet.hpp"
#include "StageInfo.hpp"

/// @brief CSVから読み取る敵情報の型です
struct ReadEnemyData
{
	int32 spawnTime;
	double r;
	double deg;
};

class Enemy
{
public:

	Enemy() = default;

	/// @brief 敵を作成します
	/// @param r 敵が出現する星からの距離です
	/// @param theta 敵が出現する場所の角度成分です
	Enemy(double r, double theta);

	/// @brief 敵を描画します
	void draw() const;

	/// @brief 敵を状態を更新します
	void update();

	/// @brief 体力を計算します
	/// @param damage 受けるダメージです
	/// @return 体力が0以下の時false
	bool damage(double damage);

	/// @brief 現在の体力を返します
	/// @return 現在の体力
	double getHP() const;

	/// @brief 弾を発射します
	/// @param eBulletArr 敵のBulletを管理する配列です
	/// @param pJetPos プレイヤーの座標です
	/// @return 弾を発射したらtureを返します
	bool shot(Array<Bullet>& eBulletArr, const Vec2& pJetPos);

	/// @brief 敵の生死を返します
	/// @return 敵が死んでいる時true
	bool isDeath() const;

	/// @brief 衝突判定のためのCircleを返します。
	/// @return 衝突判定のためのCircle
	Circle getCollider() const;

	/// @brief 現在の座標を返します
	/// @return 現在の座標
	Circular getPos() const;

private:

	/// @brief 敵の中心を返します
	/// @return 敵の中心座標
	Vec2 getCenter() const;

	double m_maxHP = 20.0;

	double m_currentHP = m_maxHP;

	Circular m_pos{ 100,0 };

	Circle m_collider{ {0,0},10 };

	Circular m_from{0,0};

	Circular m_to{0,0};

	Stopwatch stopwatch{ StartImmediately::Yes };

	static constexpr double m_eShotCoolTime = 3.0;

	int32 m_shotCnt = 1;

	double m_earthR = earthR;

	static constexpr double m_enemyHouseRange = enemyHouseRange;

	bool m_deathFlag = false;

	Anime m_explosionAnime{ TextureAsset(U"eExplosionTex"), 2, 8, 0.03,0.35,AudioAsset(U"eDeathAud") };
};
