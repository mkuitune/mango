#pragma once

namespace cma {
	namespace Messaging{

		typedef void* Context;

		void init();
		Context get_context();
		void end();
	}
}