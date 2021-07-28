
#include "Publisher.h"

Publisher::Publisher(){

    /* 
        Initiating ZMQ to publish messages into ECITS channel
        Broad direction
        Port 2121
     */

    mContext = new zmq::context_t(2);
    mPublisher = new zmq::socket_t(*mContext, ZMQ_XPUB);
    mEnvelope = "ECITS";

    srandom ((unsigned) time (NULL));

    mPublisher->bind("tcp://*:2121");
}

int Publisher::publishData(std::string feature){

    int code = s_sendmore (*mPublisher, "ECITS");
    if (code!=1){
        return code;
    }

    code = s_send (*mPublisher, feature);
    return code;
}


Publisher::~Publisher(){
    // printf("\nDestructor, you jut give me headaches\n");
    // mContext->close();
	// mPublisher->close();
	// delete mContext;
	// delete &mPublisher;
}


