#pragma once
#include <Siv3D.hpp>
#include"WorldSetting.h"

struct ReadEnemyData
{
	int enemyType;
	int movePattern;
	int shotPattern;
	int spawnTime;
	int returnTime;
	double r;
	double deg;
};
struct Bullet
{
	Circle collider;
	Vec2 position;
	Vec2 direction;
};

const double eBullet_r = 4.0;

class Enemy
{
public:
	Enemy(ReadEnemyData enemyData);
	~Enemy();
	void draw() const;
	void move();
	bool calcHP(double damage);
	void Shot(Array <Bullet> &eBulletArr,Vec2 pJetPos);
	double geteShotCoolTime();
	Vec2 getFrom();
	Vec2 getTo();
	Vec2 getPos();
	Circle getCollider();
	Vec2 r_deg{ 100,100 };
	double eShotTimer = 0.0;
	double currentHP = 30;
	double animaTime = 0.0;
	ReadEnemyData getData();

private:
	Texture tex{ U"picture/敵/GalagianArtwork/raw/enemies/kamikaze.png" };
	Circle collider{ {0,0},10 };
	Vec2 from;
	Vec2 to;
	double maxHP = 30;
	Stopwatch stopwatch{ StartImmediately::Yes };
	ReadEnemyData enemyData;
	double eShotCoolTime;
};
