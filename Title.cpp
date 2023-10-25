# include "Title.h"

Title::Title(const InitData& init)
	: IScene{ init } {}

//毎フレーム実行,主に処理
void Title::update()
{
	if (button.leftClicked())
	{
		changeScene(State::Game);
	}
}

//updateの後に実行される、主に描画
void Title::draw() const
{
	button.draw();
}
