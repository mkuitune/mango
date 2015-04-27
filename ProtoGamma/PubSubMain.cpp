#include "zmq.h"
#include "Commons/zhelpers.h"
#include <thread>
#include <iostream>
#include <string>
#include <vector>

void* ctx;
#define ADDRESS "inproc://test"

static void do_log()
{
	using namespace std;

	void* sock = zmq_socket(ctx, ZMQ_SUB);
	int rc = zmq_connect(sock, ADDRESS);
	zmq_setsockopt(sock, ZMQ_SUBSCRIBE, "", 0);
	cout << "Logger started with zmq code " << rc << endl;
	std::vector<char> buf(2054);
	while (1)
	{
		if (s_recv_str(sock, buf))
		{
			cout << "Got message " << (&buf[0]) << endl;
		}
	}
}

static void write_log()
{
	using namespace std;

	void* sock = zmq_socket(ctx, ZMQ_PUB);
	int rc = zmq_bind(sock, ADDRESS);
	while (1){
		string msg;
		std::getline(cin,msg);
		s_send(sock, msg.c_str());
		if (msg == "quit")
		{
			break;
		}
	}
	zmq_close(sock);

}

#if 0
int main(int argc, char* argv[])
{
	ctx = zmq_ctx_new();
	std::thread lis(do_log);
	std::thread write(write_log);
	write.join();
	return 0;
}
#endif