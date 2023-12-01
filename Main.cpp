# include <Siv3D.hpp> // Siv3D v0.6.12
# include"Common.h"
# include "Title.h"
# include "Game.h"

void Main()
{
	//window設定
	Window::SetStyle(WindowStyle::Sizable);
	Window::SetTitle(U"AsteriaSalvare");
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1920, 1080);
	Window::Resize(Size{ 1280, 720 });

#if not _DEBUG
	Window::SetFullscreen(true);
#endif
	System::Update();

	App manager;
	const TOMLReader toml{ U"config.toml" };
	if (not toml)
	{
		throw Error{ U"Failed to load 'config.toml'" };
	}
	manager.get()->earth_r = toml[U"WorldSetting.earth_r"].get<double>();
	manager.get()->houseSize = toml[U"WorldSetting.houseSize"].get<double>();
	manager.get()->enemyHouseRange = toml[U"WorldSetting.enemyHouseRange"].get<double>();
	manager.get()->testMode = true;
	//Asset登録
	String readSTR = toml[U"Enemy.tex"].get<String>();//自動で読み取ろうとした残骸
	TextureAsset::Register(U"enemyTex", readSTR);
	TextureAsset::Register(U"eExplosionTex", U"picture/explosion.png");
	TextureAsset::Register(U"backPic", U"picture/背景/StarryBackPic.png");
	TextureAsset::Register(U"pJetTex", U"picture/Enemy/GalagianArtwork/raw/player/ship1.png");
	TextureAsset::Register(U"shieldTex", U"picture/player/shieldPic.png");
	TextureAsset::Register(U"pBullet_tex", U"picture/Enemy/GalagianArtwork/raw/projectiles/shotsmall.png");
	TextureAsset::Register(U"eBullet_tex", U"picture/Enemy/GalagianArtwork/raw/projectiles/shotoval.png");
	TextureAsset::Register(U"enemy1_tex", U"picture/Enemy/GalagianArtwork/raw/enemies/kamikaze.png");
	TextureAsset::Register(U"Attack_Item", U"picture/Item/book_01_red.png");
	TextureAsset::Register(U"Protect_Item", U"picture/Item/book_01_blue.png");
	TextureAsset::Register(U"Special_Item", U"picture/Item/book_01_green.png");
	TextureAsset::Register(U"earthTile", U"picture/stage/maptile_jimen_sogen_02_center.png");
	TextureAsset::Register(U"titlePic", U"picture/Title/AsteriaSalvare_TitlePic.png");

	AudioAsset::Register(U"gameBGM", U"music/battleBGM.mp3");
	AudioAsset::Register(U"pShotAud", U"music/se_pyun2.mp3");
	AudioAsset::Register(U"eDeathAud", U"music/maou_se_8bit12.mp3");

	FontAsset::Register(U"townHPFont", 18, U"font/DotGothic16/DotGothic16-Regular.ttf");
	FontAsset::Register(U"titleFont", 40, U"font/DotGothic16/DotGothic16-Regular.ttf");

	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);

	manager.init(State::Game);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
