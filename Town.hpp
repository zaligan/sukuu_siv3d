#pragma once
#include<Siv3D.hpp>
#include"StageInfo.hpp"
#include"HPBar.h"
#include"Upgrade.hpp"

class Town
{
public:
	Town();


private:
	Circle m_collider{ 0,0,StageInfo::townSize};
	
	static constexpr int32 m_maxTownHP = 3000;

	HPBar m_hPBar = HPBar(m_maxTownHP);

	Upgrade m_upgrade;

	double m_attackValue = 100;

	double m_shieldHP = 100;
};
