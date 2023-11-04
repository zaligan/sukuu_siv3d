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
	/*const Texture stButton_pic{U"picture/Button/001.png"};
	const Texture exButton_pic{ U"picture/Button/005_b.png" };*/

	const Vec2 stButton_pos{ Scene::Center().x,350 };
	const Vec2 exButton_pos{ Scene::Center().x,420 };

	RoundRect st_Button{ Arg::center(stButton_pos),140,55,20 };
	RoundRect ex_Button{ Arg::center(exButton_pos),140,55,20 };

	const Audio SelectAud_Push{ U"music/決定ボタンを押す23.mp3" };
	const Audio SelectAud_Move{ U"music/カーソル移動11.mp3" };

	const Font font{ 52, U"example/font/DotGothic16/DotGothic16-Regular.ttf", FontStyle::Bitmap };

	const Texture titleBackTex{ U"picture/AsteriaSalvare　タイトル画面.jpg" };
};
