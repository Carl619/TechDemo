#pragma once
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"
#include <string>
#include <iostream>
using namespace std;

namespace NCL {
	namespace CSC8503 {
			class GolfGame;
		class TestGolfPacketReceiver : public PacketReceiver {
		public:

			TestGolfPacketReceiver(string name, GolfGame * owner) {
				this->name = name;
				this->owner = owner;
			}

			TestGolfPacketReceiver() {
			}

			void ReceivePacket(int type, GamePacket * payload, int source);
		protected:
			string name;
			GolfGame * owner;
		};
	}
}
