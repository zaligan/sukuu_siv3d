# include <Siv3D.hpp> // Siv3D v0.6.12

class Bullet
{
public:
	Bullet();
	~Bullet();

private:
	const int scale = 1;
	Vec2 bulletPos{ 0,0 };

};

Bullet::Bullet()
{
}

Bullet::~Bullet()
{
}

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });
	const Texture houseTex{ U"🏚️"_emoji };
	const Texture pJetTex{ U"picture/pJetTex.png"};
	const double pJetScale = 0.02;
	const double pSpeed = 300.0;//自機の移動速度
	const int sidePanelWidth = 200.0;//両脇パネルの横幅
	Vec2 pPos{ 300,300 };
	Circle pColi{ 0,0,33};

	const Rect ground{ 0,550,800,50 };
	const Rect leftWind{ 0,0,sidePanelWidth,600 };
	const Rect rightWind{ 800- sidePanelWidth,0,sidePanelWidth,600};
	double moveTime = 0;
	while (System::Update())
	{
		moveTime = Scene::DeltaTime();

		if (KeyA.pressed() && !KeyD.pressed()) {
			pPos.x -= moveTime * pSpeed;
		}
		else if (KeyD.pressed() && !KeyA.pressed()) {
			pPos.x += moveTime * pSpeed;
		}

		if (KeyW.pressed() && !KeyS.pressed()) {
			pPos.y -= moveTime * pSpeed;
		}
		else if (KeyS.pressed() && !KeyW.pressed()) {
			pPos.y += moveTime * pSpeed;
		}
		pPos = { Clamp(pPos.x,0.0 +sidePanelWidth,800.0-sidePanelWidth),Clamp(pPos.y,0.0,600.0) };
		pColi.setPos(Arg::center(pPos));
		pColi.draw(Palette::Black);
		pJetTex.scaled(pJetScale).draw(Arg::center(pPos));
		
		ground.draw(Palette::Brown);
		leftWind.draw(Palette::Gray);
		rightWind.draw(Palette::Gray);
		houseTex.draw(300, 460);
	}
}
