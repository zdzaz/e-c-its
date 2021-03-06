// This file is part of OpenC2X.
//
// OpenC2X is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenC2X is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenC2X.  If not, see <http://www.gnu.org/licenses/>.
//
// Authors:
// Sven Laux <slaux@mail.uni-paderborn.de>
// Gurjashan Singh Pannu <gurjashan.pannu@ccs-labs.org>
// Stefan Schneider <stefan.schneider@ccs-labs.org>
// Jan Tiemann <janhentie@web.de>


#define ELPP_THREAD_SAFE
#define ELPP_NO_DEFAULT_LOG_FILE

#include "httpServer.h"
#include "external/pbjson.hpp"
#include "external/crow_all.h"
#include <common/utility/CommunicationSender.h>
#include <common/buffers/build/trigger.pb.h>

using namespace std;

INITIALIZE_EASYLOGGINGPP

httpServer::httpServer(GlobalConfig globalConfig, string serverConfig, string loggingConf, string statisticConf) {
	mGlobalConfig = globalConfig;
	try {
		mLocalConfig.loadConfigXML(serverConfig);
	}
	catch (std::exception &e) {
		std::cerr << "Error while loading local config.xml: " << e.what() << std::endl;
	}

	std::string moduleName = "WebApplication";
	mClientCam = new CommunicationClient(moduleName, "6789", mGlobalConfig.mExpNo, loggingConf, statisticConf);
	mClientDenm = new CommunicationClient(moduleName, "6789", mGlobalConfig.mExpNo, loggingConf, statisticConf);
	mClientGps = new CommunicationClient(moduleName, "6789", mGlobalConfig.mExpNo, loggingConf, statisticConf);
	mClientObd2 = new CommunicationClient(moduleName, "6789", mGlobalConfig.mExpNo, loggingConf, statisticConf);
	mClientCamInfo = new CommunicationClient(moduleName, "6789", mGlobalConfig.mExpNo, loggingConf, statisticConf);
	mClientDccInfo = new CommunicationClient(moduleName, "6789", mGlobalConfig.mExpNo, loggingConf, statisticConf);
	mLogger = new LoggingUtility(moduleName, mGlobalConfig.mExpNo, loggingConf, statisticConf);
	// JUST DEBUGGING SELECT FEATURES $pat
	mClientEcits = new CommunicationClient(moduleName, "6789", mGlobalConfig.mExpNo, loggingConf, statisticConf);

}

httpServer::~httpServer() {
	delete mClientCam;
	delete mClientDenm;
	delete mClientGps;
	delete mClientObd2;
	delete mClientCamInfo;
	delete mClientDccInfo;
	delete mLogger;
	// JUST DEBUGGING SELECT FEATURES $pat
	delete mClientEcits;
}

//requests all CAMs from LDM
std::string httpServer::requestCam(std::string condition) {
	mMutexCam.lock();
	std::string request, reply;
	std::string serializedData;
	dataPackage::LdmData ldmData;
	//get all CAMs from LDM
	reply = mClientCam->sendRequest("CAM", condition, mLocalConfig.mTimeout);
	if (reply != "") {
		ldmData.ParseFromString(reply);

		//convert to JSON
		std::string json = "{\"type\":\"CAM\",\"number\":" + std::to_string(ldmData.data_size()) + ",\"msgs\":[";
		for (int i=0; i<ldmData.data_size(); i++) {
			std::string tempJson;
			std::string serializedCam = ldmData.data(i);
			camPackage::CAM cam;
			cam.ParseFromString(serializedCam);
			pbjson::pb2json(&cam, tempJson);
			if (i > 0) {
				json += "," + tempJson;
			}
			else {
				json += tempJson;
			}
		}
		json += "]}";
		mMutexCam.unlock();
		return json;
	}
	mMutexCam.unlock();
	return "";
}

//requests all DENMs from LDM
std::string httpServer::requestDenm(std::string condition) {
	mMutexDenm.lock();
	std::string request, reply;
	std::string serializedData;
	dataPackage::LdmData ldmData;
	//get all DENMs from LDM
	reply = mClientDenm->sendRequest("DENM", condition, mLocalConfig.mTimeout);
	if (reply != "") {
		ldmData.ParseFromString(reply);

		//convert to JSON
		std::string json = "{\"type\":\"DENM\",\"number\":" + std::to_string(ldmData.data_size()) + ",\"msgs\":[";
		for (int i=0; i<ldmData.data_size(); i++) {
			std::string tempJson;
			std::string serializedDenm = ldmData.data(i);
			denmPackage::DENM denm;
			denm.ParseFromString(serializedDenm);
			pbjson::pb2json(&denm, tempJson);
			if (i > 0) {
				json += "," + tempJson;
			}
			else {
				json += tempJson;
			}
		}
		json += "]}";
		mMutexDenm.unlock();
		return json;
	}
	mMutexDenm.unlock();
	return "";
}

// JUST DEBUGGING SELECT FEATURES $pat
Json::Value httpServer::requestFeature(std::string condition) {
	mFeaturesMutex.lock();
	Json::Value jsonData;

	//get all DENMs from LDM
	jsonData = mClientEcits->sendRequest("ECITS", condition, mLocalConfig.mTimeout);
	if (jsonData != ""){
		mFeaturesMutex.unlock();
		return jsonData;
	}
	mFeaturesMutex.unlock();
	return "sorry";
}

//requests all GPSs from LDM
std::string httpServer::requestGps(std::string condition) {
	mMutexGps.lock();
	std::string request, reply;
	std::string serializedData;
	dataPackage::LdmData ldmData;
	//get all GPSs from LDM
	reply = mClientGps->sendRequest("GPS", condition, mLocalConfig.mTimeout);
	if (reply != "") {
		ldmData.ParseFromString(reply);

		//convert to JSON
		std::string json = "{\"type\":\"GPS\",\"number\":" + std::to_string(ldmData.data_size()) + ",\"msgs\":[";
		for (int i=0; i<ldmData.data_size(); i++) {
			std::string tempJson;
			std::string serializedGps = ldmData.data(i);
			gpsPackage::GPS gps;
			gps.ParseFromString(serializedGps);
			pbjson::pb2json(&gps, tempJson);
			if (i > 0) {
				json += "," + tempJson;
			}
			else {
				json += tempJson;
			}
		}
		json += "]}";
		mMutexGps.unlock();
		return json;
	}
	mMutexGps.unlock();
	return "";
}

//requests all OBD2s from LDM
std::string httpServer::requestObd2(std::string condition) {
	mMutexObd2.lock();
	std::string request, reply;
	std::string serializedData;
	dataPackage::LdmData ldmData;
	//get all OBD2s from LDM
	reply = mClientObd2->sendRequest("OBD2", condition, mLocalConfig.mTimeout);
	if (reply != "") {
		ldmData.ParseFromString(reply);

		//convert to JSON
		std::string json = "{\"type\":\"OBD2\",\"number\":" + std::to_string(ldmData.data_size()) + ",\"msgs\":[";
		for (int i=0; i<ldmData.data_size(); i++) {
			std::string tempJson;
			std::string serializedObd2 = ldmData.data(i);
			obd2Package::OBD2 obd2;
			obd2.ParseFromString(serializedObd2);
			pbjson::pb2json(&obd2, tempJson);
			if (i > 0) {
				json += "," + tempJson;
			}
			else {
				json += tempJson;
			}
		}
		json += "]}";
		mMutexObd2.unlock();
		return json;
	}
	mMutexObd2.unlock();
	return "";
}

//requests all DCCINFOs from LDM
std::string httpServer::requestDccInfo(std::string condition) {
	mMutexDccInfo.lock();
	std::string request, reply;
	std::string serializedData;
	dataPackage::LdmData ldmData;;
	//get all dccInfos from LDM
	reply = mClientDccInfo->sendRequest("dccInfo", condition, mLocalConfig.mTimeout);
	if (reply != "") {
		ldmData.ParseFromString(reply);

		//convert to JSON
		std::string json = "";
		if(ldmData.data_size()%4 == 0) {	//only use valid information for all 4 access categories	TODO: this needs to be improved
			json = "{\"type\":\"dccInfo\",\"number\":" + std::to_string(ldmData.data_size()/4) + ",\"msgs\":[";

			for (int i=0; i<ldmData.data_size(); i+=4) {
				if(i > 0){	//not first element
					json += ",";
				}
				json += "{";
				for (int j = 0; j < 4 && i+j < ldmData.data_size(); j++){
					std::string tempJson;
					std::string serializedDccInfo = ldmData.data(i+j);
					infoPackage::DccInfo dccInfo;
					dccInfo.ParseFromString(serializedDccInfo);
					pbjson::pb2json(&dccInfo, tempJson);
					if (j > 0) {
						json += ",\"Cat"+std::to_string(j)+"\" :";
						json +=  tempJson;
					}
					else {
						json += "\"Cat"+std::to_string(j)+"\" :";
						json += tempJson;
					}
				}
				json+="}";
			}
			json += "]}";
		} else {
			mLogger->logError("received invalid DccInfo");
		}
		mMutexDccInfo.unlock();
		return json;
	}
	mMutexDccInfo.unlock();
	return "";
}

//requests all CAMINFOs from LDM
std::string httpServer::requestCamInfo(std::string condition) {
	mMutexCamInfo.lock();
	std::string request, reply;
	std::string serializedData;
	dataPackage::LdmData ldmData;
	//get all camInfos from LDM
	reply = mClientCamInfo->sendRequest("camInfo", condition, mLocalConfig.mTimeout);
	if (reply != "") {
		ldmData.ParseFromString(reply);

		//convert to JSON
		std::string json = "{\"type\":\"camInfo\",\"number\":" + std::to_string(ldmData.data_size()) + ",\"msgs\":[";

		for (int i=0; i<ldmData.data_size(); i++) {
			std::string tempJson;
			std::string serializedCamInfo = ldmData.data(i);
			infoPackage::CamInfo camInfo;
			camInfo.ParseFromString(serializedCamInfo);
			pbjson::pb2json(&camInfo, tempJson);
			if (i > 0) {
				json += "," + tempJson;
			}
			else {
				json += tempJson;
			}
		}
		json += "]}";
		mMutexCamInfo.unlock();
		return json;
	}
	mMutexCamInfo.unlock();
	return "";
}

std::string httpServer::myMac() {
	return "{\"myMac\":\"" + to_string(mGlobalConfig.mStationID) + "\"}";
}

int main(int argc, const char* argv[]){
	if(argc != 5) {
		fprintf(stderr, "missing arguments: %s <globalConfig.xml> <serverConfig.xml> <logging.conf> <statistics.conf> \n", argv[0]);
		exit(1);
	}
	crow::SimpleApp app;
	crow::logger::setLogLevel(crow::LogLevel::ERROR);	//ignore info logging in crow
	GlobalConfig config;
	try {
		config.loadConfigXML(argv[1]);
	}
	catch (std::exception &e) {
		std::cerr << "Error while loading global config.xml: " << e.what() << std::endl;
	}

	//ldm requests
	httpServer* server = new httpServer(config, argv[2], argv[3], argv[4]);

	//CAM
	CROW_ROUTE(app, "/request_cam")
	.methods("POST"_method)
	([server](const crow::request& req){

		auto request = crow::json::load(req.body);
		if (!request)
			return crow::response(400);
		std::string condition = "";
		std::string reply = "";

		if(request.has("condition")) {
			condition = request["condition"].s();
		}

		reply = server->requestCam(condition);

	    auto resp = crow::response{reply};
	    resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});

	//DENM
	CROW_ROUTE(app, "/request_denm")
	.methods("POST"_method)
	([server](const crow::request& req){

		auto request = crow::json::load(req.body);
		if (!request)
			return crow::response(400);
		std::string condition = "";
		std::string reply = "";

		if(request.has("condition")) {
			condition = request["condition"].s();
		}

		reply = server->requestDenm(condition);

	    auto resp = crow::response{reply};
	    resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});

	//Features
	CROW_ROUTE(app, "/request_feature")
	.methods("POST"_method)
	([server](const crow::request& req){
		auto request = crow::json::load(req.body);
		if (!request)
			return crow::response(200);
		std::string condition = "";
		Json::Value jsonReply = "";
		std::string serializedReply = "";
		Json::StreamWriterBuilder wbuilder;


		if(request.has("condition")) {
			condition = request["condition"].s();
		}
		// returning null, might be becuz no data in DB 
		// and that means problem is deeper
		// Fixed, it was returning null because of the way I constructed the return of requestFeature()
		jsonReply = server->requestFeature(condition);
    	serializedReply = Json::writeString(wbuilder,jsonReply);

		
	    auto resp = crow::response{serializedReply};
	    resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});


	//GPS
	CROW_ROUTE(app, "/request_gps")
	.methods("POST"_method)
	([server](const crow::request& req){

		auto request = crow::json::load(req.body);
		if (!request)
			return crow::response(400);
		std::string condition = "";
		std::string reply = "";

		if(request.has("condition")) {
			condition = request["condition"].s();
		}

		reply = server->requestGps(condition);

	    auto resp = crow::response{reply};
	    resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});

	//OBD2
	CROW_ROUTE(app, "/request_obd2")
	.methods("POST"_method)
	([server](const crow::request& req){

		auto request = crow::json::load(req.body);
		if (!request)
			return crow::response(400);
		std::string condition = "";
		std::string reply = "";

		if(request.has("condition")) {
			condition = request["condition"].s();
		}

		reply = server->requestObd2(condition);

	    auto resp = crow::response{reply};
	    resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});

	//DccInfo
	CROW_ROUTE(app, "/request_dccinfo")
	.methods("POST"_method)
	([server](const crow::request& req){

		auto request = crow::json::load(req.body);
		if (!request)
			return crow::response(400);
		std::string condition = "";
		std::string reply = "";

		if(request.has("condition")) {
			condition = request["condition"].s();
		}

		reply = server->requestDccInfo(condition);

	    auto resp = crow::response{reply};
	    resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});

	//CamInfo
	CROW_ROUTE(app, "/request_caminfo")
	.methods("POST"_method)
	([server](const crow::request& req){

		auto request = crow::json::load(req.body);
		if (!request)
			return crow::response(400);
		std::string condition = "";
		std::string reply = "";

		if(request.has("condition")) {
			condition = request["condition"].s();
		}

		reply = server->requestCamInfo(condition);

	    auto resp = crow::response{reply};
	    resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});

	//denm triggering
	CommunicationSender* senderToDenm = new CommunicationSender("WebApplication", "1111", config.mExpNo, argv[3], argv[4]);
	CROW_ROUTE(app, "/trigger_denm")
	.methods("POST"_method)
	([senderToDenm](const crow::request& req){

		auto request = crow::json::load(req.body);
		if (!request)
			return crow::response(400);
		triggerPackage::TRIGGER trigger;
		std::string serializedTrigger;
		std::string content = "";

		if(request.has("content")) {
			content = request["content"].s();
		}

		trigger.set_content(content);
		trigger.SerializeToString(&serializedTrigger);

		senderToDenm->send("TRIGGER", serializedTrigger);

	    auto resp = crow::response{"Triggered DENM"};
	    resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});

	//gets own mac
	CROW_ROUTE(app, "/my_mac")
	.methods("GET"_method)
	([server](){
		auto resp = crow::response{server->myMac()};
		resp.add_header("Access-Control-Allow-Origin","*");
		resp.add_header("Content-Type","application/json");
		return resp;
	});
	

	app.port(1188).multithreaded().run();
	delete server;
	delete senderToDenm;
}
