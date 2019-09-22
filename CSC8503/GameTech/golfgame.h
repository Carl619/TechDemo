#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../CSC8503Common/menu.h"
#include "../CSC8503Common/player.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"
#include "../CSC8503Common/testgolfpacketreceiver.h"
class PathFinding;

namespace NCL {
	namespace CSC8503 {
		class GolfGame {
		public:
			GolfGame();
			~GolfGame();

			virtual void UpdateGame(float dt);
			bool isServer() { return ServerOnline; }
			void addhit() { hits++; }
			void setTotalHIts(int hits) { this->hits = hits; }
			int getTotalHIts() { return hits; }
			void setHighScore(bool score) { highscore = score; }
		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on).
			*/
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitSphereCollisionTorqueTest();
			void InitCubeCollisionTorqueTest();
			void InitSphereAABBTest();
			void InitGJKWorld();
			void BridgeConstraintTest();
			void SimpleGJKTest();
			void SimpleAABBTest();
			void SimpleAABBTest2();
			void ChangeLevel();
			bool SelectObject();
			void MoveSelectedObject();
			void initLevel1();
			void initLevel2();
			void initLevel3();
			void play();
			bool checkServer();
			void DisplayPathfinding(std::vector<Vector3> testNodes);
			bool highscore;
			
			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& size);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			int			limithit;
			int			hits;
			float		forceMagnitude;
			float		Height;
			GameObject* selectionObject = nullptr;

			OGLMesh*	cubeMesh = nullptr;
			OGLMesh*	sphereMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader*	basicShader = nullptr;
			bool ServerOnline = false;
			int port;
			GameServer * server;
			GameClient * client;

			TestGolfPacketReceiver  serverReceiver;
			TestGolfPacketReceiver  clientReceiver;
			std::vector<Menu*> Menus;
			std::vector<Player*> players;
			int level;
		};
	}
}

