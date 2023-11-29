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

	const Audio SelectAud_Push{ U"music/決定ボタンを押す23.mp3" };
	
};
