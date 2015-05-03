#pragma once

#include "SDL2/SDL.h"

#include <iostream>

namespace cma{
	std::ostream& sdl_keycode_to_stream(std::ostream& os, Sint32 kc);

}