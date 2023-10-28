# pragma once
# include "Common.h"

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init);

	void update() override;

	void draw() const override;

private:
	RoundRect button_S{ 325,350,150,60,10 };
	RoundRect button_E{ 325,420,150,60,10 };

	Font font{ 35 };

	const Texture titleBackTex{ U"picture/AsteriaSalvare　タイトル画面.jpg" };
};
