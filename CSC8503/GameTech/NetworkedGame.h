#pragma once
#include "TutorialGame.h"
namespace NCL {
	namespace CSC8503 {
		class NetworkedGame : public TutorialGame {
		public:
			void  UpdateAsClient(float dt);

			void   BroadcastSnapshot(bool  deltaFrame);
		};
	}
}