#include "testgolfpacketreceiver.h"
#include "../GameTech/golfgame.h"

#include <iostream>
#include <fstream>
#include "../../Common/Assets.h"

#include "gamestatemachine.h"


using namespace NCL;
using namespace CSC8503;

void TestGolfPacketReceiver::ReceivePacket(int type, GamePacket * payload, int source) {
	if (type == String) {
		StringPacket * realPacket = (StringPacket *)payload;
		string msg = realPacket->GetStringFromData();
		if (owner->isServer()) {
			if (msg == "ended") {
				NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::ONLINERESULTSERVER);
				std::ifstream infile(Assets::DATADIR + "scores.txt");
				int highScore = 99999;
				string line;
				if (infile.is_open())
				{
					if (getline(infile, line)) {
						highScore = stoi(line);
					}
				}
				infile.close();
				if (owner->getTotalHIts() < highScore) {
					cout << "Player got a new record";
					std::ofstream myfile;
					myfile.open(Assets::DATADIR + "scores.txt");
					myfile << to_string(owner->getTotalHIts());
					myfile.close();
					owner->setHighScore(true);
				}
			}
			else if (msg == "disconnect") {
				NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::ONLINESERVER);
				owner->setHighScore(false);
				owner->setTotalHIts(0);
			}
			else
			owner->addhit();
		}
		else {
			if (msg == "ended") {
				if (NCL::CSC8503::GameStateMachine::getInstance()->getActiveState()!= NCL::CSC8503::GameState::ONLINERESULTCLIENT) {
					NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::ONLINERESULTCLIENT);
				}
			}
			else if (msg == "highended") {
				if (NCL::CSC8503::GameStateMachine::getInstance()->getActiveState() != NCL::CSC8503::GameState::ONLINERESULTCLIENT) {
					NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::ONLINERESULTCLIENT);
				
					
				}	
				Debug::Print("You got a new record", Vector2(200, 200), Vector4(0, 1, 0, 1));
			}
			else
				owner->setTotalHIts(std::stoi(msg));
		}
		cout << name << " received message : " << msg << std::endl;

	}

}