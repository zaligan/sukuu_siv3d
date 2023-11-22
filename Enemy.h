#pragma once
#include <Siv3D.hpp>
#include"Anime.h"

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
	Enemy(const ReadEnemyData& enemyData,double earth_r,double enemyHouseRange);
	~Enemy();
	void draw() const;
	void move();
	bool calcHP(double damage);
	void Shot(Array <Bullet> &eBulletArr,const Vec2& pJetPos);
	double geteShotCoolTime();
	Vec2 getFrom();
	Vec2 getTo();
	Vec2 getPos();
	Circle getCollider();
	Vec2 r_deg{ 100,100 };
	ReadEnemyData getData();
	Audio eDeathAud{ U"music/maou_se_8bit12.mp3" };
	Texture eExplosion_tex{ U"picture/explosion.png" };
	double eShotTimer = 0.0;
	double currentHP;
	Anime explosion_Anime{ eDeathAud,eExplosion_tex, 8, 2, 0.03,0.35 };

private:
	Texture tex{ U"picture/敵/GalagianArtwork/raw/enemies/kamikaze.png" };
	Circle collider{ {0,0},10 };
	Vec2 from;
	Vec2 to;
	double maxHP = 20.0;
	Stopwatch stopwatch{ StartImmediately::Yes };
	ReadEnemyData enemyData;
	double eShotCoolTime;
	double earth_r;
	double enemyHouseRange;
};
