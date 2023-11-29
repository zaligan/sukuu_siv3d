# include "Title.h"

Title::Title(const InitData& init)
	: IScene{ init }
{
	
}

//毎フレーム実行,主に処理
void Title::update()
{
	ClearPrint();
	if (KeyJ.pressed())
	{
		SelectAud_Push.play();
		changeScene(State::Game);
	}
}

//updateの後に実行される、主に描画
void Title::draw() const
{
	TextureAsset(U"titlePic").resized(1920,1080).drawAt(Scene::Center());
	FontAsset(U"titleFont")(U"Press J to Continue").drawAt(960,670);
}
