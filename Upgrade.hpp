#pragma once

/// @brief アップグレードの種類と数を管理します
struct Upgrade
{
	int32 Attack = 0;
	int32 Defense = 0;
	int32 Special = 0;

	Upgrade& operator+=(const Upgrade& other)
	{
		Attack += other.Attack;
		Defense += other.Defense;
		Special += other.Special;
		return *this;
	}
};
