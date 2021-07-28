// Try with Drawio, fresh mind

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

#include <string>
#include <thread>
#include <chrono>
#include <zmq.hpp>
#include "../OpenC2X/common/utility/external/zhelpers.hpp"

class Publisher{
	
	public:
		Publisher();
		~Publisher();
		int publishData(std::string feature);

	private:
		zmq::context_t *mContext;
		zmq::socket_t *mPublisher;
		std::__cxx11::string mEnvelope;

};
