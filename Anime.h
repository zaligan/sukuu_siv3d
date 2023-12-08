#pragma once
#include<Siv3D.hpp>

/// @brief スプライトシートを用いてアニメーションを表示します
class Anime
{
public:
	/// @brief Animeを作成します
	/// @param texture 表示するテクスチャです
	/// @param rowSize textureが持つコマの行数です
	/// @param majorSize textureが持つコマの列数です
	/// @param frmTime １コマの描画時間です(秒)
	Anime(const Texture& texture, int32 rowSize, int32 majorSize, double frmTime) :
		Anime(texture, majorSize, rowSize, frmTime, 1.0) {}

	/// @brief Animeを作成します
	/// @param texture 表示するテクスチャです
	/// @param rowSize textureが持つコマの行数です
	/// @param majorSize textureが持つコマの列数です
	/// @param frmTime １コマの描画時間です(秒)
	/// @param resize テクスチャの表示倍率です
	Anime(const Texture& texture, int32 rowSize, int32 majorSize, double frmTime, double resize) :
		m_texture(texture),
		m_majorSize(majorSize),
		m_rowSize(rowSize),
		m_frmTime(frmTime),
		m_resize(resize),
		m_index({ 0,0 }) {}

	/// @brief Animeを作成します
	/// @param texture 表示するテクスチャです
	/// @param rowSize textureが持つコマの行数です
	/// @param majorSize textureが持つコマの列数です
	/// @param frmTime １コマの描画時間です(秒)
	/// @param resize テクスチャの表示倍率です
	/// @param audioPath 再生時に鳴らす効果音です
	Anime(const Texture& texture, int32 rowSize, int32 majorSize, double frmTime, double resize, const Audio& audio) :
		m_texture(texture),
		m_majorSize(majorSize),
		m_rowSize(rowSize),
		m_frmTime(frmTime),
		m_resize(resize),
		m_index({ 0,0 }),
		m_audio(audio) {}

	/// @brief テクスチャのアニメーション、効果音の再生を行います。
	/// @return アニメーションの再生が終わるとtrueを返します
	bool update()
	{
		if (m_index.x == m_majorSize - 1 && m_index.y == m_rowSize-1)
		{
			return true;
		}

		stopwatch.start();
		const double time = stopwatch.sF();

		if (m_audioFlag && m_audio)
		{
			m_audio.playOneShot();
			m_audioFlag = false;
		}

		if (time > m_frmTime)
		{
			stopwatch.restart();
			++m_index.x;

			if (m_index.x >= m_majorSize)
			{
				m_index.x = 0;
				++m_index.y;
			}
		}
		return false;
	}

	/// @brief アニメーションを１コマ目までリセットします
	void reset()
	{
		m_index = { 0,0 };
	}

	/// @brief アニメーションを描画します
	/// @param pos 描画する座標です
	void drawAt(const Vec2& pos) const
	{
		m_texture.uv(static_cast<double>(m_index.x) / m_majorSize, static_cast<double>(m_index.y) / m_rowSize, 1.0 / m_majorSize, 1.0 / m_rowSize).scaled(m_resize).drawAt(pos);
	}

	/// @brief 回転したアニメーションを描画します
	/// @param pos 描画する座標です
	/// @param theta 回転させる角度です
	void drawAt(const Vec2& pos,double theta) const
	{
		m_texture.uv(static_cast<double>(m_index.x) / m_majorSize, static_cast<double>(m_index.y) / m_rowSize, 1.0 / m_majorSize, 1.0 / m_rowSize).scaled(m_resize).rotated(theta).drawAt(pos);
	}

private:

	Texture m_texture;

	Audio m_audio;

	int32 m_majorSize = 0;

	int32 m_rowSize = 0;

	double m_frmTime = 0;

	double m_resize = 1.0;

	//音声を１回だけ鳴らすためのフラグ変数です
	bool m_audioFlag = true;

	//表示するコマの{列、行}です
	Point m_index = { 0,0 };

	Stopwatch stopwatch{ StartImmediately::No };
};
