#pragma once
# include "Common.h"

// タイトルシーン
class Result : public App::Scene
{
public:

	Result(const InitData& init);

	void update() override;

	void draw() const override;

private:
	
};
