#pragma once
#include <Siv3D.hpp>
#include "Anime.h"
#include "Bullet.hpp"

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
	/// @param earth_r 星の半径です敵が出現する原点からの距離は earth_r + r となります
	/// @param enemyHouseRange 敵が侵入できない家のエリア半径です
	Enemy(double r, double theta, double earth_r, double enemyHouseRange);

	/// @brief 敵を描画します
	void draw() const;

	/// @brief 敵を移動します
	void move();

	/// @brief 体力を計算します
	/// @param damage 受けるダメージです
	/// @return 体力が0以下の時false
	bool calcHP(double damage);

	/// @brief 弾を発射します
	/// @param eBulletArr 敵のBulletを管理する配列です
	/// @param pJetPos プレイヤーの座標です
	/// @return 弾を発射したらtureを返します
	bool shot(Array<Bullet>& eBulletArr, const Vec2& pJetPos);

	/// @brief 敵の生死を返します
	/// @return 敵が死んでいる時true
	bool checkDeath() const;

	/// @brief 衝突判定のためのCircleを返します。
	/// @return 衝突判定のためのCircle
	Circle getCollider() const;

	/// @brief 現在の座標を返します
	/// @return 現在の座標
	Circular getPos() const;
	
	Anime explosion_Anime{ TextureAsset(U"eExplosionTex"), 2, 8, 0.03,0.35,AudioAsset(U"eDeathAud")};

private:
	double currentHP;
	Circular pos{ 100,0 };
	Circle collider{ {0,0},10 };
	Circular from;
	Circular to;
	double maxHP = 20.0;
	Stopwatch stopwatch{ StartImmediately::Yes };
	static constexpr double eShotCoolTime = 3.0;
	double earth_r = 0;
	double enemyHouseRange;
	bool deathFlag = false;
	void init(double r, double degree);
	int32 shotCnt = 1;
	Vec2 getCenter() const;
};
