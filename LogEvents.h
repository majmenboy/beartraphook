#pragma once
 
#include <deque>
#include "Entities.h"
#include "Glowmanager.h"
 
struct loginfo
{
	loginfo(std::string text,
		D3DXCOLOR color,
		float time)
	{
		this->text = text;
		this->color = color;
		this->time = time;
	}
 
	std::string text;
	D3DXCOLOR color;
	float time;
};
 
namespace LogEvents
{
	void FireGameEvent(IGameEvent* pEvent);
	void Draw();
 
	static std::deque<loginfo>events;
}