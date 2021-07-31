#include "Consensus.h"

int TIME_EXCEEDED = 0;

Consensus::Consensus() {

    printf("Gathering Consensus...\n");
    Publisher mPublisher();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
}


Consensus::~Consensus() {
    // this might solve the weird behaviour (threads and stuff)
	// mThreadEcitsSender->join();
	// delete mThreadEcitsSender;
}


void Consensus::init(std::string serialFeature){

    /* 
        Futurely, use threads (Libboost) to avoid weird behaviours?
     */
    // mThreadEcitsSender = new boost::thread(&Ecits::sendEcits, this);

    Json::Value feature;
    Json::Value majFeature;
	Json::Reader reader;
	reader.parse(serialFeature,feature);
    int feature_id = feature["id"].asInt();

    // SEND PROPOSALS
    sendProposal(serialFeature);
    // RECEIVE PROMISES
    string serialMajFeat = receivePromises(feature_id);
    reader.parse(serialMajFeat,majFeature);

    // SEND ACCEPT REQUESTS
    try
    {

        majFeature["id"] = feature_id;
        majFeature.removeMember("phase");
        sendAcceptRequest(majFeature);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    

    TIME_EXCEEDED = 0;
    mN_replies = 0;

}
// PHASE 1
void Consensus::sendProposal(std::string serialFeature){
    printf("Sending proposal to reachable nodes with Publisher\n");
    int code = mPublisher.publishData(serialFeature);
}

// 2 AUX Functions for RECEIVING PROMISES
void timeout_handler(int signum){
    cout << "\n************ Broke by timeout ************\n" << endl;
    TIME_EXCEEDED=1;
}

string promises_comparison(std::vector<std::string> promises_received){

    map<string, int> feature_count;
    pair<string, int> maxValue = make_pair("{}", 0);

    for(std::size_t i = 0; i < promises_received.size(); i++) {
               
        if(feature_count.find(promises_received[i]) == feature_count.end()){
            feature_count.insert(std::pair<string,int>(promises_received[i],1));
        } else {
            feature_count[promises_received[i]]++;
        }

        for(map<string,int>::const_iterator it = feature_count.begin(); it != feature_count.end(); ++it)
        {
            if(it->second > maxValue.second){
                maxValue.first = it->first;
            }
        }
    }

    return maxValue.first;

}

// PHASE 2
string Consensus::receivePromises(int feature_id){

    signal(SIGALRM, timeout_handler);
    std::vector<std::string> promises;

    cout << "\n___________________RECV PROMISES Phase 2_____________________________\n" << endl;
   /* 
        Receive Phase 2 messages
        socket()
        setsockopt() --> setting timeout for recvfrom
        bind()
        recvfrom() --> while have not received at least 3 responses 
                    or timout has not been reached
    */

    int sockid = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addrport;

    addrport.sin_family = AF_INET;
    addrport.sin_port = htons(2100);
    addrport.sin_addr.s_addr = htonl(INADDR_ANY);

    struct timeval time;
	time.tv_sec = atoi("10"); // Pay attention to anti-core
	time.tv_usec =  0;
	setsockopt(sockid, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time)); 

    int status = bind(sockid, (struct sockaddr *) &addrport, sizeof(addrport));
    printf("Receive Promises Status Socket Bind: %d\n",status);
    
    mN_replies = 0;
    
    //This delay is needed due to not using liboost
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Timeout alarm; 1 Second
    alarm(1);

    /* 
        Wait for a minimum of min_reply replies and Timoeout --> Stop RECVFROM
        mN_replies = global counter tracking the number of replies
     */ 
    int min_reply = 1;
    while(mN_replies < min_reply && !TIME_EXCEEDED){
        char receive_buf[200];
        int count = recvfrom(sockid, receive_buf, sizeof(receive_buf), 0,0, 0);
        mN_replies++;

        if(count>0){
            receive_buf[count] = 0;
            printf("\nReply Number %d\nPushing into promises!", mN_replies);
            promises.push_back(receive_buf);
        }
    }

    status=close(sockid);
    printf("Receive Promises Status Socket Close: %d\n",status);


    return promises_comparison(promises);

}

// PHASE 3
void Consensus::sendAcceptRequest(Json::Value feature){

    cout << "\n___________________SEND ACCEPT REQUEST WITH MAJ FEATURE_____________________________\n" << endl;
    
    Json::StreamWriterBuilder wbuilder;
    std::string serializedFeature = Json::writeString(wbuilder,feature);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "Phase 3 Feature: "<< serializedFeature << "\n" << endl;
    int code = mPublisher.publishData(serializedFeature);
}   


