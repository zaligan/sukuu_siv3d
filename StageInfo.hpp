#pragma once
//ステージの情報をまとめたヘッダーです

namespace StageInfo
{
	//ステージの星の半径
	constexpr int32 earthR = 300;

	//街の大きさ
	constexpr double townSize = 60.0;

	//敵が侵入できない街のエリア半径
	constexpr double enemyHouseRange = 200.0;

	//画面外に出た弾が削除される範囲
	constexpr int32 bulletDeleteRange = 1000;
}
