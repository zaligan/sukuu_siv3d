# pragma once
# include "Common.h"

// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
	const Texture cat{ U"🐈"_emoji };
	const double earth_r = 400.0;
	Vec2 pJet_pos{0,0};
	const double pJet_speed = 0.6;
	double deltaTime = 0.0;
	double degrees = 0.0;
	double radians = 0.0;

	// 2D カメラ
	// 初期設定: 中心 (0, 0), ズームアップ倍率 1.0
	Camera2D camera{ Vec2{ 0, 0 }, 1.0 };
	Mat3x2 mat = Mat3x2::Identity();

	const Texture pJetTex{ U"picture/敵/GalagianArtwork/raw/enemies/fighter.png" };
};
