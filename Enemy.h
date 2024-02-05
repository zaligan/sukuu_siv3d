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
	/// @return 体力が0以下の時,true
	bool damage(double damage);

	/// @brief 現在の体力を返します
	/// @return 現在の体力
	double getHP() const;

	/// @brief 弾を発射します
	/// @param eBulletArr 敵のBulletを管理する配列です
	/// @param pJetPos プレイヤーの座標です
	/// @return 弾を発射したら,trueを返します
	bool shot(Array<Bullet>& eBulletArr, const Vec2& pJetPos);

	/// @brief 敵の生死を返します
	/// @return 敵が死んでいる時,true
	bool isDeath() const;

	/// @brief 衝突判定のためのCircleを返します。
	/// @return 衝突判定のためのCircle
	Circle getCollider() const;

	/// @brief 敵の中心を返します
	/// @return 敵の中心座標
	Circular getCenter() const;

	/// @brief 衝突した弾が初めての衝突か確認して、初めての場合IDをArrayに追加します
	/// @param bulletID 衝突した弾のIDです
	/// @return 衝突したことのある弾だった場合,trueを返します
	bool isHitThisBullet(int32 bulletID);

private:

	/// @brief 最大体力です
	double m_maxHP = 10.0;

	/// @brief 現在の体力です
	double m_currentHP = m_maxHP;

	/// @brief 自身の中心の座標です
	Circular m_pos{ 100,0 };

	/// @brief 衝突範囲です
	Circle m_collider{ {0,0},10 };

	/// @brief 出現座標です
	Circular m_from{0,0};

	/// @brief 移動先座標です
	Circular m_to{0,0};

	/// @brief スポーンしてからの時間を計ります
	Stopwatch stopwatch{ StartImmediately::Yes };

	/// @brief 攻撃力です
	static constexpr double m_bulletDamage = 10.0;

	/// @brief 攻撃間隔(秒)です
	static constexpr double m_eShotCoolTime = 3.0;

	/// @brief スポーンしてから攻撃した合計数です
	int32 m_shotCnt = 0;

	/// @brief 敵が街に近づけない範囲です
	static constexpr double m_enemyHouseRange = StageInfo::enemyHouseRange;

	/// @brief 自身の体力が０以下の時,trueとなります
	bool m_deathFlag = false;

	/// @brief 破壊された時のアニメーションです
	Anime m_explosionAnime{ TextureAsset(U"eExplosionTex"), 2, 8, 0.03,0.35,AudioAsset(U"eDeathAud") };

	/// @brief 受けたプレイヤーの貫通弾の個別IDを記録します
	Array<int32> m_hitEnhancedBulletArr;
};
