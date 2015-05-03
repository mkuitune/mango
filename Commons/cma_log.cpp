#include "cma_log.h"

namespace cma {
	std::shared_ptr<Log> Log::s_log;
	std::shared_ptr<std::thread> Log::s_logThread;
	std::atomic<bool> Log::s_run_log; // TODO replace with messaging
	void* Log::s_msg_context;//

}