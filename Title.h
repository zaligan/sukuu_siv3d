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
	Rect button{ 300,300,300,80 };
	
};
