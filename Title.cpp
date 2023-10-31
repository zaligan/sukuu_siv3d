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
	button_S.draw();
	button_E.draw();

	button_SP.scaled(0.6).drawAt(400, 380);/*001_02.pngの時はscaled(0.7)*/
	button_EP.scaled(0.6).drawAt(400, 450);

	font(U"START").draw(340, 350, Palette::White);
	font(U"EXIT").draw(360, 420, Palette::White);
}
