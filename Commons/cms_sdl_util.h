#pragma once

#include "SDL2/SDL.h"

#include <iostream>

namespace cms{
	std::ostream& SDLKeycodeToStream(std::ostream& os, Sint32 kc);

}