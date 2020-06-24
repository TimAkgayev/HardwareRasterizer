#pragma once
#include <string>
#include <vector>

#include "Terrain.h"
#include "CharacterController.h"
#include "WorldObject.h"
#include "PhysicsEngine.h"
#include "Shaders.h"
#include "Skybox.h"


class SceneManager
{
public:
	SceneManager(ID3D11Device* device);
	~SceneManager();

	CharacterController* GetPlayer() const;
	Terrain* GetTerrain() const;
	PhysicsEngine* GetPhysicsEngine() const;

	void Update(float dt);
	void LoadLevel(std::string filename);

	void Render();

private:
	CharacterController* mPlayer;
	Terrain* mTerrain;
	std::vector<WorldObject*> mWorldObjectList;
	PhysicsEngine* mPhysicsEngine;
	Skybox* mSkybox;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;

	void mClearOldLevel();
};