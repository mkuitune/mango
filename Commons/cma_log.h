#pragma once

#include "zhelpers.h"
#include "zmq.h"
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <atomic>

namespace cma {

	//#define CMA_LOG_SOCKET "tcp://localhost:5558"

	// PUB/SUB channel for logging
#define CMA_LOG_SOCKET "inproc://log"

	// PUB/SUB channel for events from windowing thread
#define CMA_UIEVENT_SOCKET "inproc://uievent"

	// PUB/SUB channel for modification to windowing state
#define CMA_UIMODIFY_SOCKET "inproc://uimodify"

	class LogConfig{
	public:
		const static std::string logfile;
	};

	class Log{
	public:
		enum Mode {
			WriteStdout = 1, WriteFile = 2
		};

		std::ofstream file;
		int m_mode;
		bool m_overwrite;
		Log(){
			m_mode = WriteStdout | WriteFile;
			m_overwrite = true;
		}

		void open(){
			if (m_overwrite)
				file.open(LogConfig::logfile);
			else
				file.open(LogConfig::logfile, std::ios::app);
		}

		void write(const char* msg){
			if (m_mode & WriteFile)
				file << msg << std::endl;
			if (m_mode & WriteStdout)
				std::cout << msg << std::endl;
		}

		~Log(){
			file.close();
		}
		//
		// Logging resources
		//
		static std::shared_ptr<Log> s_log;
		static std::shared_ptr<std::thread> s_logThread;
		static std::atomic<bool> s_run_log; // TODO replace with messaging
		static void* s_msg_context;

		static void log_runner()
		{
			// Open server socket
			//		void* context = zmq_ctx_new(); // Can use this only if forked...
			void* receiver = zmq_socket(s_msg_context, ZMQ_SUB);
			//		zmq_bind(receiver, CMA_LOG_SOCKET);
			zmq_connect(receiver, CMA_LOG_SOCKET);
			zmq_setsockopt(receiver, ZMQ_SUBSCRIBE, "", 0);

			std::vector<char> msgBuf(2056);
			Log::s_log->write("Logger service started");
			while (s_run_log){
				// Read messagces from log socket
				// Write message to currently active log
				bool succ = s_recv_str(receiver, msgBuf);
				if (succ)
				{
					s_log->write(&msgBuf[0]);
				}
			}
			// Close server socket
			zmq_close(receiver);
			//		zmq_ctx_destroy(context);
			s_log->write("Logger service finalized.");
		}

		static void init(){
			// Create context
			s_msg_context = zmq_ctx_new();
			// Create log pointer
			s_log.reset(new Log());
			s_log->open();
			// Start log thread
			s_run_log = true;
			s_logThread.reset(new std::thread(log_runner));
		}
		static void end(){
			s_run_log = false;
			s_logThread->join();
			zmq_ctx_destroy(s_msg_context);
		}
	};

	/// Use only from one thread.
	class LogWriter {
	public:
		void* m_sender;
		void* m_context;

		LogWriter() :m_context(Log::s_msg_context), m_sender(0){
		}

		~LogWriter(){
			// Close ZMQ client socket
			zmq_close(m_sender);
			//zmq_ctx_destroy(m_context);
		}

		void open(){
			if (m_sender) return;
			m_sender = zmq_socket(m_context, ZMQ_PUB);
			//zmq_connect(m_sender, CMA_LOG_SOCKET);
			zmq_bind(m_sender, CMA_LOG_SOCKET);
		}

		void operator()(const std::string& msg){ log(msg); }
		void operator()(const char* msg){ log(msg); }
		void operator()(const std::string& fst, const std::string& snd){ log(fst + snd); }
		void operator()(const std::string& fst, const std::string& snd,
			const std::string& thrd){
			log(fst + snd + thrd);
		}
		void operator()(const std::string& fst, const std::string& snd,
			const std::string& thrd, const std::string& fourth){
			log(fst + snd + thrd + fourth);
		}

		void log(const std::string& msg){
			log(msg.c_str());
		}

		// ZMQ client socket
		void log(const char* message){
			size_t threadHash = std::hash<std::thread::id>()(std::this_thread::get_id());
			std::string fullMessage = "thread:" + std::to_string(threadHash) + " " + std::string(message);
			if (m_sender){
				s_send(m_sender, fullMessage.c_str());
				//				s_send_noblock(m_sender, fullMessage.c_str());
			}
		}
	};
}