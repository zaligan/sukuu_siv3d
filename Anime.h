#pragma once
#include<Siv3D.hpp>
class Anime
{
public:
	Anime(const Texture& texture, int majorSize, int rowSize, int frame);
	Anime(const Audio& audio,const Texture& texture, int majorSize, int rowSize, int frame);
	Anime(const Audio& audio,const Texture& texture, int majorSize, int rowSize, int frame,double resize);
	bool update();
	void draw(const Vec2& pos) const;

private:
	Texture m_texture;
	Audio m_audio;
	int m_majorSize;
	int m_rowSize;
	int m_frame;
	Vec2 m_index;
	int m_count;
	double m_resize = 1.0;
};
