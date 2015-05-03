#include "cma_messaging.h"
#include "zmq.h"
#include <atomic>

namespace cma {
	namespace Messaging{

		std::atomic<Context> s_msg_context(0);
		void init(){
			if (s_msg_context) return;

			s_msg_context = zmq_ctx_new();
		}
		
		void* get_context(){
			return s_msg_context;
		}

		void end(){
			if (!s_msg_context) return;

			zmq_ctx_destroy(s_msg_context);
			s_msg_context = 0;
		}
	}
}
