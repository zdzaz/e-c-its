
#include "aux/Publisher.h"
// #include "/usr/local/include/boost/thread/thread.hpp"
#include <mutex>
#include <jsoncpp/json/json.h>

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>


#include <string>
#include <iostream>
#include <thread>

#include <signal.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using std::endl;	

/*
 * Class that handles the ECITS algorithm (Phases 1,2 and 3)
 */

class Consensus {

public:
	Consensus();
	~Consensus();

	void init(std::string feature);
	// Phase 1
	void sendProposal(std::string feature);
	// Phase 2
	string receivePromises(int feature_id);
	// Phase 3
	void sendAcceptRequest(Json::Value maj_feat);


private:
    Publisher mPublisher;
    int mN_replies;
	// boost::thread* mThreadEcitsSender;
	long mIdCounter;

	std::string mSerializedFeature;
	Json::Value mFeature;

	int FIRSTTIME = 1;
};

