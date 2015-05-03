#pragma once

#include "SDL2/SDL.h"

#include <iostream>

namespace cma{
	std::ostream& SDLKeycodeToStream(std::ostream& os, Sint32 kc);

}