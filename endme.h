#pragma once
#include "Utilities.h"

float random_between_two_floats(float min, float max)
{
	return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}

#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include "Hooks.h"
#include <iostream>
#include "UTIL Functions.h"

