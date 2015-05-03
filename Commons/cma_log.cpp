#include "cma_log.h"

namespace cma {
	const std::string LogConfig::logfile = std::string("log.txt");

	std::shared_ptr<Log> Log::s_log;
	std::shared_ptr<std::thread> Log::s_logThread;
	std::atomic<bool> Log::s_run_log; // TODO replace with messaging
}