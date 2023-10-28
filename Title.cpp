# include "Title.h"

Title::Title(const InitData& init)
	: IScene{ init } {}

//毎フレーム実行,主に処理
void Title::update()
{
	if (button_S.leftClicked())
	{
		changeScene(State::Game);
	}
	if (button_E.leftClicked())
	{
		System::Exit();
	}
}

//updateの後に実行される、主に描画
void Title::draw() const
{
	Scene::SetBackground(ColorF{ 0.9, 0.95, 1.0 });
	titleBackTex.scaled(0.9).drawAt(Scene::Center());
	button_S.draw(Palette::Orange);
	button_E.draw(Palette::Skyblue);

	font(U"はじめる").draw(330,355);
	font(U"おわる").draw(350, 425);
}
