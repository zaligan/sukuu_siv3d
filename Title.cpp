# include "Title.h"

Title::Title(const InitData& init)
	: IScene{ init } {}

//毎フレーム実行,主に処理
void Title::update()
{
	if (st_Button.leftClicked())
	{
		SelectAud_Push.play();
		changeScene(State::Game);
	}
	if (ex_Button.leftClicked())
	{
		System::Exit();
	}
	/*if (st_Button.mouseOver() || ex_Button.mouseOver())カーソルが上に乗った瞬間だけ音を出すにしたい
	{
		SelectAud_Move.playOneShot();
	}*/
}

//updateの後に実行される、主に描画
void Title::draw() const
{
	titleBackTex.scaled(0.9).drawAt(Scene::Center());
	/*st_Button.draw();*/
	/*ex_Button.draw();/*後で消す*/

	/*stButton_pic.scaled(0.6).drawAt(stButton_pos);/*001_02.pngの時はscaled(0.7)*/
	/*exButton_pic.scaled(0.6).drawAt(exButton_pos); */
	if (st_Button.mouseOver())
	{
		font(U"START").draw(Arg::center(stButton_pos), Palette::White);
	}
	else
	{
		font(U"START").draw(Arg::center(stButton_pos), Palette::Gray);
	}
	if (ex_Button.mouseOver())
	{
		font(U"EXIT").draw(Arg::center(exButton_pos), Palette::White);
	}
	else
	{
		font(U"EXIT").draw(Arg::center(exButton_pos), Palette::Gray);
	}
}
