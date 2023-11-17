#include "StarrySkyBackground.h"

StarrySkyBackground::StarrySkyBackground()
{
}

StarrySkyBackground::~StarrySkyBackground()
{
}

void StarrySkyBackground::draw() const
{
	Scene::SetBackground(HSV{ 267, 0.99, 0.25 });
}
