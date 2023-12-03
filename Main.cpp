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
	Window::Resize(Size{ 640, 360 });

#if not _DEBUG
	Window::SetFullscreen(true);
#endif
	System::Update();

	//シーンマネージャーを作成
	App manager;

	//調整用無敵モード、通常プレイ時はコメントアウト
	manager.get()->testMode = true;

	//Asset登録
	TextReader reader{ U"asset-list.txt" };
	if (not reader)
	{
		throw Error{ U"Failed to load 'asset-list.txt'" };
	}

	String line;
	//１行ずつ読み込む
	while (reader.readLine(line))
	{
		//tab区切りでitemsに格納
		const Array<String> items = line.split(U'\t');
		//Fontの時は長さ４、その他は長さ３かどうかチェック
		if ((items[0] == U"Font" && items.size() != 4) || (items[0] != U"Font" && items.size() != 3))
		{
			throw Error{ U"Invalid asset-list.txt" };
		}
		//アセットの型
		const String& type = items[0];
		//アセットに付ける名前
		const String& assetName = items[1];
		//アセットとして登録するファイルのパス
		const String& path = items[2];

		//Textureアセットの登録
		if (type == U"Texture")
		{
			TextureAsset::Register(assetName, path);
			continue;
		}

		//Audioアセットの登録
		if (type == U"Audio")
		{
			AudioAsset::Register(assetName, path);
			continue;
		}

		//Fontアセットの登録
		if (type == U"Font")
		{
			//items[3]はフォントの大きさ
			const int32 fontSize = Parse<int32>(items[3]);
			FontAsset::Register(assetName, fontSize , path);
			continue;
		}
	}
	//シーンの追加
	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);

	//Gameシーンから開始する場合このコメントを外す
	manager.init(State::Game);

	//メインループ
	while (System::Update())
	{
		//現在のシーンを実行
		//シーンに実装された.update()次に.draw()の順で実行される
		if (not manager.update())
		{
			break;
		}
	}
}
