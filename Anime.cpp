#include "Anime.h"


Anime::Anime(const Texture& texture, int majorSize, int rowSize, double frmTime) :
	m_texture(texture),
	m_majorSize(majorSize),
	m_rowSize(rowSize),
	m_frmTime(frmTime),
	m_index({ 0,0 }),
	m_count(0) {}

Anime::Anime(const Texture & texture, int majorSize, int rowSize, double frmTime, double resize) :
	m_texture(texture),
	m_majorSize(majorSize),
	m_rowSize(rowSize),
	m_frmTime(frmTime),
	m_resize(resize),
	m_index({ 0,0 }),
	m_count(0){}

Anime::Anime(const String& audioPath, const Texture& texture, int majorSize, int rowSize, double frmTime, double resize):
	m_audio(Audio{audioPath}),
	m_texture(texture),
	m_majorSize(majorSize),
	m_rowSize(rowSize),
	m_frmTime(frmTime),
	m_resize(resize),
	m_index({ 0,0 }),
	m_count(0){}

bool Anime::update()
{
	stopwatch.start();
	const double time = stopwatch.sF();
	if (m_index.x == 0 && m_index.y == 0 && m_audio)
		m_audio.play();
	if (time > m_frmTime)
	{
		stopwatch.restart();
		++m_index.x;

		if (m_index.x >= m_majorSize)
		{
			m_index.x = 0;
			++m_index.y;
			if (m_index.y >= m_rowSize)
			{
				m_index.y = 0;
				return true;
			}
		}
	}
	return false;
}

void Anime::draw(const Vec2& pos) const
{
	m_texture.uv(static_cast<double>(m_index.x) / m_majorSize, static_cast<double>(m_index.y) / m_rowSize, 1.0 / m_majorSize, 1.0 / m_rowSize).scaled(m_resize).draw(Arg::center(pos));
}

