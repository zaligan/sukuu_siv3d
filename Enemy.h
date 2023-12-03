﻿#pragma once
#include <Siv3D.hpp>
#include"Anime.h"
#include "Bullet.h"

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


const double eBullet_r = 4.0;

class Enemy
{
public:
	Enemy(const ReadEnemyData& enemyData,double earth_r,double enemyHouseRange);
	Enemy(const Vec2& spawn,double earth_r,double enemyHouseRange);
	~Enemy();
	void init(Vec2 spawn);
	void draw() const;
	void move();
	bool calcHP(double damage);
	void Shot(Array<Bullet> &eBulletArr,const Vec2& pJetPos);
	bool getDeathFlag();
	double geteShotCoolTime();
	Vec2 getFrom();
	Vec2 getTo();
	Vec2 getPos();
	Circle getCollider();
	ReadEnemyData getData();
	Vec2 r_deg{ 100,100 };
	double eShotTimer = 0.0;
	double currentHP;
	Anime explosion_Anime{ (U"music/maou_se_8bit12.mp3"),TextureAsset(U"eExplosionTex"), 8, 2, 0.03,0.35};

private:
	Circle collider{ {0,0},10 };
	Vec2 from;
	Vec2 to;
	double maxHP = 20.0;
	Stopwatch stopwatch{ StartImmediately::Yes };
	ReadEnemyData enemyData;
	double eShotCoolTime;
	double earth_r;
	double enemyHouseRange;
	bool deathFlag = false;
};
