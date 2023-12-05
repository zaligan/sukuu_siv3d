#pragma once
#include <Siv3D.hpp>
#include"Anime.h"
#include "Bullet.hpp"

struct ReadEnemyData
{
	int spawnTime;
	double r;
	double deg;
};

class Enemy
{
public:

	Enemy() = default;

	Enemy(double r, double degree, double earth_r, double enemyHouseRange);

	void draw() const;
	void move();
	bool calcHP(double damage);
	void shot(Array<Bullet>& eBulletArr, const Vec2& pJetPos);
	bool checkDeath();
	double geteShotCoolTime();
	Circle getCollider();

	// TODO: deg のほうをラジアンにして、将来的には Vec2 → Circular に変更する
	Vec2 r_deg{ 100,0 };
	double eShotTimer = 0.0;
	double currentHP;
	Anime explosion_Anime{ TextureAsset(U"eExplosionTex"), 2, 8, 0.03,0.35,(U"music/maou_se_8bit12.mp3") };

private:

	Circle collider{ {0,0},10 };
	Vec2 from;
	Vec2 to;
	double maxHP = 20.0;
	Stopwatch stopwatch{ StartImmediately::Yes };
	static constexpr double eShotCoolTime = 3.0;
	double earth_r;
	double enemyHouseRange;
	bool deathFlag = false;

	void init(double r, double degree);

	Vec2 getCenter() const;
};
