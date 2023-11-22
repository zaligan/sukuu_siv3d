#pragma once
#include<Siv3D.hpp>
class Anime
{
public:
	Anime(const Texture& texture, int majorSize, int rowSize, double frmTime);
	Anime(const Audio& audio,const Texture& texture, int majorSize, int rowSize, double frmTime);
	Anime(const Audio& audio,const Texture& texture, int majorSize, int rowSize, double frmTime,double resize);
	bool update();
	void draw(const Vec2& pos) const;

private:
	Texture m_texture;
	Audio m_audio;
	int m_majorSize;
	int m_rowSize;
	double m_frmTime;
	Vec2 m_index;
	int m_count;
	double m_resize = 1.0;
	Stopwatch stopwatch{ StartImmediately::No };
};
