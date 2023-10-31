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
	RoundRect button_S{285,350,230,55,20};
	RoundRect button_E{ 285,420,230,55,20 };
	const Texture button_SP{ U"picture/Button/001.png" };
	const Texture button_EP{ U"picture/Button/005_b.png" };

	Font font{ 40 };

	const Texture titleBackTex{ U"picture/AsteriaSalvare　タイトル画面.jpg" };
};
