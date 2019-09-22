#include "golfgame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/gamestatemachine.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/mainmenu.h"
#include "../CSC8503Common/resultmenu.h"
#include "../CSC8503Common/levelmenu.h"

#include "../CSC8503Common/hole.h"
#include "../CSC8503Common/rotaryobstacle.h"
#include "../CSC8503Common/wallbostacle.h"
#include "../CSC8503Common/enemyobstacle.h"
#include "../CSC8503Common/bridgeobstacle.h"
#include "../CSC8503Common/enemyai.h"

#include "../CSC8503Common/NavigationGrid.h"

#include <iostream>
#include <fstream>
#include "../../Common/Assets.h"

using namespace NCL;
using namespace CSC8503;

GolfGame::GolfGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;
	Menus.push_back(new MainMenu("Main Menu"));
	Menus.push_back(new LevelMenu("Level Menu"));
	Menus.push_back(new ResultMenu("Result Menu"));
	Debug::SetRenderer(renderer);
	Height = 1;
	ServerOnline = false;
	level = 0;
	limithit = 100;
	hits = 0;
	highscore = false;
	NetworkBase::Initialise();
	
	
	port = NetworkBase::GetDefaultPort();
	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it

*/
void GolfGame::InitialiseAssets() {
	cubeMesh = new OGLMesh("cube.msh");
	cubeMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	cubeMesh->UploadToGPU();

	sphereMesh = new OGLMesh("sphere.msh");
	sphereMesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	sphereMesh->UploadToGPU();

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

GolfGame::~GolfGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
	NetworkBase::Destroy();
	Menus.clear();
}

void GolfGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}
	if (GameStateMachine::getInstance()->getActiveState() == GameState::MAINMENU) {
		ChangeLevel();
		MainMenu * mainMenu = dynamic_cast<MainMenu*>(Menus[0]);
		if (mainMenu) {
			mainMenu->controller();
			if (mainMenu->getCUrrentOption() == 0) {
				Debug::Print("--> Game", Vector2(200, 150), Vector4(1, 1, 0, 1));
				Debug::Print("Online", Vector2(200, 100), Vector4(0, 0, 0, 1));
				Debug::Print("Exit", Vector2(200, 50), Vector4(0, 0, 0, 1));
			}
			else if (mainMenu->getCUrrentOption() == 1) {
				Debug::Print("Game", Vector2(200, 150), Vector4(0,0, 0, 1));
				Debug::Print("--> Online", Vector2(200, 100), Vector4(1, 1, 0, 1));
				Debug::Print("Exit", Vector2(200, 50), Vector4(0, 0, 0, 1));
			}
			else if (mainMenu->getCUrrentOption() == 2) {
				Debug::Print("Game", Vector2(200, 150), Vector4(0, 0, 0, 1));
				Debug::Print("Online", Vector2(200, 100), Vector4(0, 0, 0, 1));
				Debug::Print("--> Exit", Vector2(200, 50), Vector4(1, 1, 0, 1));
			}
		}
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::RESULTMENU) {
		ChangeLevel();
		ResultMenu * mainMenu = dynamic_cast<ResultMenu*>(Menus[2]);
		if (mainMenu) {
			mainMenu->controller();
			if (hits > limithit) {
				Debug::Print("You lose", Vector2(200, 150), Vector4(1, 0, 0, 1));
			}
			else
				Debug::Print("You win", Vector2(200, 150), Vector4(0, 1, 0, 1));
			if (mainMenu->getCUrrentOption() == 0) {
			
				Debug::Print("--> Replay", Vector2(200, 100), Vector4(1,1, 0, 1));
				Debug::Print("Exit", Vector2(200, 50), Vector4(0, 0, 0, 1));
			}
			else if (mainMenu->getCUrrentOption() == 1) {
				Debug::Print("Replay", Vector2(200, 100), Vector4(0, 0, 0, 1));
				Debug::Print("--> Exit", Vector2(200, 50), Vector4(1, 1, 0, 1));
			}
		}
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::LEVELMENU) {
		ChangeLevel();
		LevelMenu * mainMenu = dynamic_cast<LevelMenu*>(Menus[1]);
		if (mainMenu) {
			mainMenu->controller();
			if (mainMenu->getCUrrentOption() == 0) {
				Debug::Print("--> Level 1", Vector2(200, 200), Vector4(1, 1, 0, 1));
				Debug::Print("Level 2", Vector2(200, 150), Vector4(0, 0, 0, 1));
				Debug::Print("Level 3", Vector2(200, 100), Vector4(0, 0, 0, 1));
				Debug::Print("Back", Vector2(200, 50), Vector4(0, 0, 0, 1));
			}
			else if (mainMenu->getCUrrentOption() == 1) {
				Debug::Print("Level 1", Vector2(200, 200), Vector4(0, 0, 0, 1));
				Debug::Print("--> Level 2", Vector2(200, 150), Vector4(1, 1,0, 1));
				Debug::Print("Level 3", Vector2(200, 100), Vector4(0, 0, 0, 1));
				Debug::Print("Back", Vector2(200, 50), Vector4(0,0, 0, 1));
			}
			else if (mainMenu->getCUrrentOption() == 2) {
				Debug::Print("Level 1", Vector2(200, 200), Vector4(0, 0, 0, 1));
				Debug::Print("Level 2", Vector2(200, 150), Vector4(0, 0, 0, 1));
				Debug::Print("--> Level 3", Vector2(200, 100), Vector4(1, 1, 0, 1));
				Debug::Print("Back", Vector2(200, 50), Vector4(0, 0, 0, 1));
			}
			else if (mainMenu->getCUrrentOption() == 3) {
				Debug::Print("Level 1", Vector2(200, 200), Vector4(0, 0, 0, 1));
				Debug::Print("Level 2", Vector2(200, 150), Vector4(0, 0, 0, 1));
				Debug::Print("Level 3", Vector2(200, 100), Vector4(0, 0, 0, 1));
				Debug::Print("--> Back", Vector2(200, 50), Vector4(1, 1,0, 1));
			}
		}
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::LEVEL1) {
		NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::LOADING);
		level = 1;
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::LEVEL2) {
		NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::LOADING);
		level = 2;
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::LEVEL3) {
		NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::LOADING);
		level = 3;
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::LOADING) {
		if (level == 1)
			initLevel1();
		else if (level == 2)
			initLevel2();
		else if (level == 3)
			initLevel3();
		GameStateMachine::getInstance()->replace(GameState::GAME);
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::EXIT) {
		
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::ONLINE) {
		highscore = false;
		hits = 0;
		checkServer();

	}

	else if (GameStateMachine::getInstance()->getActiveState() == GameState::ONLINESERVER){
		server->SendGlobalPacket(
			StringPacket(std::to_string(hits)));
		server->UpdateServer();

	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::ONLINERESULTCLIENT) {
		client->UpdateClient();
		ResultMenu * mainMenu = dynamic_cast<ResultMenu*>(Menus[2]);
		if (mainMenu) {
			mainMenu->controller();
			if (hits > limithit) {
				Debug::Print("You lose", Vector2(200, 150), Vector4(1, 0, 0, 1));
			}
			else
				Debug::Print("You win", Vector2(200, 150), Vector4(0, 1, 0, 1));
			if (mainMenu->getCUrrentOption() == 0) {

				Debug::Print("--> Replay", Vector2(200, 100), Vector4(1, 1, 0, 1));
				Debug::Print("Exit", Vector2(200, 50), Vector4(0, 0, 0, 1));
			}
			else if (mainMenu->getCUrrentOption() == 1) {
				Debug::Print("Replay", Vector2(200, 100), Vector4(0, 0, 0, 1));
				Debug::Print("--> Exit", Vector2(200, 50), Vector4(1, 1, 0, 1));
			}
		}
	}
	else if (GameStateMachine::getInstance()->getActiveState() == GameState::DISCONNECTCLIENT) {
		client->SendPacket(StringPacket("disconnect"));
		client->UpdateClient();
		client->Destroy();
		GameStateMachine::getInstance()->popState();
	}
	else 	if (GameStateMachine::getInstance()->getActiveState() == GameState::ONLINECHECKRESULTS) {
		client->SendPacket(StringPacket("ended"));
		client->UpdateClient();
		ChangeLevel();
	}

	else if (GameStateMachine::getInstance()->getActiveState() == GameState::ONLINERESULTSERVER) {
		if(highscore)
			server->SendGlobalPacket(
				StringPacket("highended"));
		else
	server->SendGlobalPacket(
		StringPacket("ended"));
	server->UpdateServer();

	}
	else {
		UpdateKeys();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O)) {
			Height += 10*dt;
			if (Height > 10)
				Height = 10;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) {
			Height -= 10*dt;
			if (Height < 0)
				Height = 0;
		}

		Debug::Print("Actual Elevation: " + std::to_string(Height), Vector2(10, 60), Vector4(0, 0, 0, 1));

		Debug::Print("Number Hits: " + std::to_string(players[0]->getHits()), Vector2(10, 80), Vector4(0, 0, 0, 1));

		if (useGravity) {
			Debug::Print("(G)ravity on", Vector2(10, 40), Vector4(0, 0, 0, 1));
		}
		else {
			Debug::Print("(G)ravity off", Vector2(10, 40), Vector4(0, 0, 0, 1));
		}

		SelectObject();
		MoveSelectedObject();
		world->UpdateWorld(dt);
		renderer->Update(dt);
		physics->Update(dt);
		players[0]->respawn();
		if (players[0]->getHits() > limithit) {
			GameStateMachine::getInstance()->replace(GameState::RESULTMENU);
			hits = players[0]->getHits();
		}

		if (GameStateMachine::getInstance()->getActiveState() == GameState::ONLINECLIENT) {
			client->UpdateClient();
		}
	}
	

	Debug::FlushRenderables();
	renderer->Render();
}

void GolfGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_R)) {
		players[0]->HaveToRespawn();
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_V)) {
		world->GetMainCamera()->SetPosition(players[0]->GetConstTransform().GetWorldPosition());
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::KEYBOARD_G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_F8)) {
		world->ShuffleObjects(false);
	}
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 100, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -100, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(100, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -100));
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 100));
		}
	}
}

void GolfGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(3.0f);
	world->GetMainCamera()->SetFarPlane(4200.0f);
	world->GetMainCamera()->SetPitch(-35.0f);
	world->GetMainCamera()->SetYaw(320.0f);
	world->GetMainCamera()->SetPosition(Vector3(-50, 120, 200));
}

void GolfGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	//InitCubeGridWorld(2, 2, 50.0f, 50.0f, Vector3(10, 10, 10));
	//InitSphereGridWorld(w, 10, 10, 50.0f, 50.0f, 10.0f);

	//InitSphereGridWorld(w, 1, 1, 50.0f, 50.0f, 10.0f);
	//InitCubeGridWorld(w,1, 1, 50.0f, 50.0f, Vector3(10, 10, 10));
	//InitCubeGridWorld(w, 1, 1, 50.0f, 50.0f, Vector3(8, 8, 8));

	//InitSphereCollisionTorqueTest(w);
	//InitCubeCollisionTorqueTest(w);

	//InitSphereGridWorld(w, 1, 1, 50.0f, 50.0f, 10.0f);
	//BridgeConstraintTest(w);
	//InitGJKWorld(w);

	//DodgyRaycastTest(w);
	//InitGJKWorld(w);
	//InitSphereAABBTest(w);
	//SimpleGJKTest(w);
	//SimpleAABBTest2(w);

	//InitSphereCollisionTorqueTest(w);
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* GolfGame::AddFloorToWorld(const Vector3& position, const Vector3& size) {
	GameObject* floor = new GameObject();

	AABBVolume* volume = new AABBVolume(size);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(size);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple'
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* GolfGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* GolfGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

void GolfGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, radius, z * rowSpacing);
			AddSphereToWorld(position, radius);
		}
	}
}

void GolfGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 10.0f;
	Vector3 cubeDims = Vector3(10, 10, 10);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, cubeDims.y, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
	AddFloorToWorld(Vector3(0, -100, 0), Vector3(10000, 10, 500));
}

void GolfGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, cubeDims.y, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(10, -100, 1), Vector3(10000, 10, 500));
}


void GolfGame::InitSphereCollisionTorqueTest() {
	AddSphereToWorld(Vector3(15, 0, 0), 10.0f);
	AddSphereToWorld(Vector3(-25, 0, 0), 10.0f);
	AddSphereToWorld(Vector3(-50, 0, 0), 10.0f);

	AddCubeToWorld(Vector3(-50, 0, -50), Vector3(60, 10, 10), 10.0f);

	AddFloorToWorld(Vector3(0, -100, 0), Vector3(10000, 10, 500));
}


void GolfGame::InitCubeCollisionTorqueTest() {
	Vector3 cubeSize(10, 10, 10);
	AddCubeToWorld(Vector3(15, 0, 0), cubeSize, 10.0f);
	AddCubeToWorld(Vector3(-25, 0, 0), cubeSize, 10.0f);
	AddCubeToWorld(Vector3(-50, 0, 0), cubeSize, 10.0f);

	AddCubeToWorld(Vector3(-50, 0, -50), Vector3(60, 10, 10), 10.0f);

	AddFloorToWorld(Vector3(0, -100, 0), Vector3(10000, 10, 500));
}

void GolfGame::InitSphereAABBTest() {
	Vector3 cubeSize(10, 10, 10);

	AddCubeToWorld(Vector3(0, 0, 0), cubeSize, 10.0f);
	AddSphereToWorld(Vector3(2, 0, 0), 5.0f, 10.0f);
}

void GolfGame::InitGJKWorld() {
	Vector3 dimensions(20, 2, 10);
	float inverseMass = 10.0f;

	for (int i = 0; i < 2; ++i) {
		GameObject* cube = new GameObject();

		OBBVolume* volume = new OBBVolume(dimensions);

		cube->SetBoundingVolume((CollisionVolume*)volume);

		cube->GetTransform().SetWorldPosition(Vector3(0, 0, 0));
		cube->GetTransform().SetWorldScale(dimensions);

		if (i == 1) {
			cube->GetTransform().SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), 90.0f));
		}

		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
		cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

		cube->GetPhysicsObject()->SetInverseMass(inverseMass);
		cube->GetPhysicsObject()->InitCubeInertia();

		world->AddGameObject(cube);
	}
}

void GolfGame::BridgeConstraintTest() {
	float sizeMultiplier = 1.0f;

	Vector3 cubeSize = Vector3(8, 8, 8) * sizeMultiplier;

	int numLinks = 5;

	GameObject* start = AddCubeToWorld(Vector3(0, 0, 0), cubeSize, 0);

	GameObject* end = AddCubeToWorld(Vector3((numLinks + 2) * 20 * sizeMultiplier, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(Vector3((i + 1) * 20 * sizeMultiplier, 0, 0), cubeSize, 10.0f);
		PositionConstraint* constraint = new PositionConstraint(previous, block, 30.0f);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, 30.0f);
	world->AddConstraint(constraint);
}

void GolfGame::SimpleGJKTest() {
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* fallingCube = AddCubeToWorld(Vector3(0, 20, 0), dimensions, 10.0f);
	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);

	delete fallingCube->GetBoundingVolume();
	delete newFloor->GetBoundingVolume();

	fallingCube->SetBoundingVolume((CollisionVolume*)new OBBVolume(dimensions));
	newFloor->SetBoundingVolume((CollisionVolume*)new OBBVolume(floorDimensions));

}

void GolfGame::SimpleAABBTest() {
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(100, 2, 100);

	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);
	GameObject* fallingCube = AddCubeToWorld(Vector3(10, 20, 0), dimensions, 10.0f);
}

void GolfGame::SimpleAABBTest2() {
	Vector3 dimensions = Vector3(5, 5, 5);
	Vector3 floorDimensions = Vector3(8, 2, 8);

	GameObject* newFloor = AddCubeToWorld(Vector3(0, 0, 0), floorDimensions, 0.0f);
	GameObject* fallingCube = AddCubeToWorld(Vector3(8, 20, 0), dimensions, 10.0f);
}

void NCL::CSC8503::GolfGame::ChangeLevel()
{
	InitCamera();
	world->ClearAndErase();
	physics->Clear();
	selectionObject = nullptr;
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool GolfGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(10, 0));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::MOUSE_LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(10, 0));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void GolfGame::MoveSelectedObject() {
	renderer->DrawString("Click Force:" + std::to_string(forceMagnitude), Vector2(10, 20));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::MOUSE_RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if(selectionObject!=nullptr){
			if (world->Raycast(ray, closestCollision, true)) {
				if (closestCollision.node == selectionObject) {
					Player * pla = dynamic_cast<Player*>( selectionObject);
					if (pla) {
						if (GameStateMachine::getInstance()->getActiveState() == GameState::ONLINECLIENT) {
							client->SendPacket(StringPacket(std::to_string(1)));
							client->UpdateClient();
							pla->setHits(hits);
						}
						else {
							pla->addHit();
						}
						selectionObject->GetPhysicsObject()->AddForceAtPosition(
							Vector3(ray.GetDirection().x, Height, ray.GetDirection().z) * forceMagnitude,
							closestCollision.collidedAt);

					}
				}
			}
		}
	}
}

void NCL::CSC8503::GolfGame::initLevel1()
{
	InitCamera();

	world->ClearAndErase();
	physics->Clear();
	players.clear();

	Hole * ho = new Hole();
	Vector3 holeSize = Vector3(5, 5, 5);
	Vector3 position = Vector3(1000, -80, 50);
	AABBVolume* holevolume = new AABBVolume(holeSize);
	ho->SetBoundingVolume((CollisionVolume*)holevolume);
	ho->GetTransform().SetWorldScale(holeSize);
	ho->GetTransform().SetWorldPosition(position);

	ho->SetRenderObject(new RenderObject(&ho->GetTransform(), cubeMesh, basicTex, basicShader));
	ho->SetPhysicsObject(new PhysicsObject(&ho->GetTransform(), ho->GetBoundingVolume()));

	ho->GetPhysicsObject()->SetInverseMass(0);
	ho->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(ho);

	RotaryObstacle *rot = new RotaryObstacle();
	Vector3 rotSize = Vector3(20, 175, 50);
	rot->setAxis(RotationAxis::X);
	Vector3 rotposition = Vector3(200, 100,0);
	OBBVolume* rotvolume = new OBBVolume(rotSize);
	rot->SetBoundingVolume((CollisionVolume*)rotvolume);
	rot->GetTransform().SetWorldScale(rotSize);
	rot->GetTransform().SetWorldPosition(rotposition);

	rot->SetRenderObject(new RenderObject(&rot->GetTransform(), cubeMesh, basicTex, basicShader));
	rot->SetPhysicsObject(new PhysicsObject(&rot->GetTransform(), rot->GetBoundingVolume()));

	rot->GetPhysicsObject()->SetInverseMass(0);
	rot->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(rot);

	RotaryObstacle *rot2 = new RotaryObstacle();
	Vector3 rot2Size = Vector3(20, 175, 50);
	rot2->setAxis(RotationAxis::X);
	Vector3 rot2position = Vector3(800, 100, 0);
	OBBVolume* rot2volume = new OBBVolume(rot2Size);
	rot2->SetBoundingVolume((CollisionVolume*)rot2volume);
	rot2->GetTransform().SetWorldScale(rot2Size);
	rot2->GetTransform().SetWorldPosition(rot2position);

	rot2->SetRenderObject(new RenderObject(&rot2->GetTransform(), cubeMesh, basicTex, basicShader));
	rot2->SetPhysicsObject(new PhysicsObject(&rot2->GetTransform(), rot2->GetBoundingVolume()));

	rot2->GetPhysicsObject()->SetInverseMass(0);
	rot2->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(rot2);

	Player* pla = new Player();
	float radius = 5;
	Vector3 plaposition = Vector3(5, 5, 5);
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	pla->SetBoundingVolume((CollisionVolume*)volume);
	pla->GetTransform().SetWorldScale(sphereSize);
	pla->GetTransform().SetWorldPosition(plaposition);
	pla->SetRenderObject(new RenderObject(&pla->GetTransform(), sphereMesh, basicTex, basicShader));
	pla->SetPhysicsObject(new PhysicsObject(&pla->GetTransform(), pla->GetBoundingVolume()));
	pla->GetPhysicsObject()->SetInverseMass(90);
	pla->GetPhysicsObject()->InitSphereInertia();
	pla->setOriginalPosition(plaposition);
	world->AddGameObject(pla);
	players.push_back(pla);
	ho->setPlayers(players);

	WallObstacle * wall = new WallObstacle();
	Vector3 wallSize = Vector3(10, 100, 500);
	Vector3 wallposition = Vector3(600, 4, 75);
	AABBVolume* wallvolume = new AABBVolume(wallSize);
	wall->SetBoundingVolume((CollisionVolume*)wallvolume);
	wall->GetTransform().SetWorldScale(wallSize);
	wall->GetTransform().SetWorldPosition(wallposition);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();
	wall->setOriginalPosition(wallposition);
	world->AddGameObject(wall);
	AddFloorToWorld(Vector3(10, -100, 1), Vector3(10000, 10, 100));
}

void NCL::CSC8503::GolfGame::initLevel2()
{
	InitCamera();

	world->ClearAndErase();
	physics->Clear();
	players.clear();

	Hole * ho = new Hole();
	Vector3 holeSize = Vector3(5, 5, 5);
	Vector3 position = Vector3(1050, -80, 50);
	AABBVolume* holevolume = new AABBVolume(holeSize);
	ho->SetBoundingVolume((CollisionVolume*)holevolume);
	ho->GetTransform().SetWorldScale(holeSize);
	ho->GetTransform().SetWorldPosition(position);

	ho->SetRenderObject(new RenderObject(&ho->GetTransform(), cubeMesh, basicTex, basicShader));
	ho->SetPhysicsObject(new PhysicsObject(&ho->GetTransform(), ho->GetBoundingVolume()));

	ho->GetPhysicsObject()->SetInverseMass(0);
	ho->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(ho);

	EnemyObstacle *ene = new EnemyObstacle();
	Vector3 eneSize = Vector3(5, 5, 5);
	Vector3 eneposition = Vector3(200, -80, 75);
	AABBVolume* enevolume = new AABBVolume(eneSize);
	ene->SetBoundingVolume((CollisionVolume*)enevolume);
	ene->GetTransform().SetWorldScale(eneSize);
	ene->GetTransform().SetWorldPosition(eneposition);
	ene->SetRenderObject(new RenderObject(&ene->GetTransform(), cubeMesh, basicTex, basicShader));
	ene->SetPhysicsObject(new PhysicsObject(&ene->GetTransform(), ene->GetBoundingVolume()));
	ene->GetPhysicsObject()->SetInverseMass(0);
	ene->GetPhysicsObject()->InitCubeInertia();
	ene->setCenter(eneposition);
	world->AddGameObject(ene);

	Player* pla = new Player();
	float radius = 5;
	Vector3 plaposition = Vector3(5, 5, 5);
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	pla->SetBoundingVolume((CollisionVolume*)volume);
	pla->GetTransform().SetWorldScale(sphereSize);
	pla->GetTransform().SetWorldPosition(plaposition);
	pla->SetRenderObject(new RenderObject(&pla->GetTransform(), sphereMesh, basicTex, basicShader));
	pla->SetPhysicsObject(new PhysicsObject(&pla->GetTransform(), pla->GetBoundingVolume()));
	pla->GetPhysicsObject()->SetInverseMass(90);
	pla->GetPhysicsObject()->InitSphereInertia();
	pla->setOriginalPosition(plaposition);
	world->AddGameObject(pla);
	players.push_back(pla);
	ho->setPlayers(players);
	ene->setObjecive(players[0]);

	AddFloorToWorld(Vector3(10, -100, 1), Vector3(500, 10, 500));

	BridgeObstacle * brid = new BridgeObstacle();
	Vector3 cubeSize = Vector3(8, 8, 8) * brid->getSizeMultiplier();
	GameObject* start = AddCubeToWorld(Vector3(550, -100, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(Vector3((brid->getnumLinks() + 2) * 20 * brid->getSizeMultiplier() + start->GetTransform().GetLocalPosition().x + 200, -100, 0), cubeSize, 0);
	GameObject* previous = start;
	for (int i = 0; i < brid->getnumLinks(); ++i) {
		GameObject* block = AddCubeToWorld(Vector3((i + 1) * 20 * brid->getSizeMultiplier() + previous->GetTransform().GetLocalPosition().x, -100, 0), cubeSize, 10.0f);
		PositionConstraint* constraint = new PositionConstraint(previous, block, 30.0f);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, 30.0f);
	world->AddConstraint(constraint);
	delete brid;
//	world->AddGameObject(brid);

	AddFloorToWorld(Vector3(1050, -100, 1), Vector3(100, 20, 100));
}

void NCL::CSC8503::GolfGame::initLevel3()
{
	InitCamera();

	world->ClearAndErase();
	physics->Clear();
	players.clear();

	Hole * ho = new Hole();
	Vector3 holeSize = Vector3(5, 5, 5);
	Vector3 position = Vector3(1000, -80, 500);
	AABBVolume* holevolume = new AABBVolume(holeSize);
	ho->SetBoundingVolume((CollisionVolume*)holevolume);
	ho->GetTransform().SetWorldScale(holeSize);
	ho->GetTransform().SetWorldPosition(position);

	ho->SetRenderObject(new RenderObject(&ho->GetTransform(), cubeMesh, basicTex, basicShader));
	ho->SetPhysicsObject(new PhysicsObject(&ho->GetTransform(), ho->GetBoundingVolume()));

	ho->GetPhysicsObject()->SetInverseMass(0);
	ho->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(ho);
	float radius = 5;
	EnemyAI *ene = new EnemyAI();
	Vector3 eneSize = Vector3(radius, radius, radius);
	Vector3 eneposition = Vector3(200, -70, 175);
	SphereVolume* enevolume = new SphereVolume(radius);
	ene->SetBoundingVolume((CollisionVolume*)enevolume);
	ene->GetTransform().SetWorldScale(eneSize);
	ene->GetTransform().SetWorldPosition(eneposition);
	ene->SetRenderObject(new RenderObject(&ene->GetTransform(), sphereMesh, basicTex, basicShader));
	ene->SetPhysicsObject(new PhysicsObject(&ene->GetTransform(), ene->GetBoundingVolume()));
	ene->GetPhysicsObject()->SetInverseMass(1);
	ene->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(ene);

	Player* pla = new Player();
	
	Vector3 plaposition = Vector3(400, 5, 200);
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	pla->SetBoundingVolume((CollisionVolume*)volume);
	pla->GetTransform().SetWorldScale(sphereSize);
	pla->GetTransform().SetWorldPosition(plaposition);
	pla->SetRenderObject(new RenderObject(&pla->GetTransform(), sphereMesh, basicTex, basicShader));
	pla->SetPhysicsObject(new PhysicsObject(&pla->GetTransform(), pla->GetBoundingVolume()));
	pla->GetPhysicsObject()->SetInverseMass(90);
	pla->GetPhysicsObject()->InitSphereInertia();
	pla->setOriginalPosition(plaposition);
	world->AddGameObject(pla);
	players.push_back(pla);
	ho->setPlayers(players);
	ene->addPlayer(players[0]);
	AddFloorToWorld(Vector3(10, -100, 1), Vector3(1000, 10, 1000));


}

void NCL::CSC8503::GolfGame::play()
{
	initLevel1();
}

bool NCL::CSC8503::GolfGame::checkServer()
{
	std::ifstream infile(Assets::DATADIR + "serveronline.txt");
	int on = 0;
	infile >> on;
	infile.close();
	if (on == 0) {
		std::ofstream myfile;
		myfile.open(Assets::DATADIR + "serveronline.txt");
		myfile << "1";
		myfile.close();
		server = new GameServer(port, 1);
		serverReceiver = TestGolfPacketReceiver("Server", this);
		server->RegisterPacketHandler(String, &serverReceiver);
		ServerOnline = true;
		NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::ONLINESERVER);
		return true;
	}
	else if (on == 1) {
		client = new GameClient();
		clientReceiver = TestGolfPacketReceiver("Client", this);
		client->RegisterPacketHandler(String, &clientReceiver);
		bool canConnect = client->Connect(127, 0, 0, 1, port);
		
		NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::ONLINECLIENT);
		initLevel1();
		return true;
	}
	return false;
}

void NCL::CSC8503::GolfGame::DisplayPathfinding(std::vector<Vector3> testNodes)
{
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));

	}
}
