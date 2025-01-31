#ifndef __ENGINE_CORE_H__
#define __ENGINE_CORE_H__
#pragma once


// hekbas: Include here all headers needed in Editor
// Include in Editor when needed: #include "../TheOneEngine/EngineCore.h"
#include "Defs.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"

#include <chrono>
#include <memory>


class EngineCore
{
public:

	EngineCore();

	void Awake();
	void Start();

	void Update(double dt);

	void Render(Camera* camera);

	void DrawAxis();
	void DrawGrid(int grid_size, int grid_step);
	void DrawFrustum(const glm::mat4& viewProjectionMatrix);

	// (x, y) Indicate the bottom left corner1
	void OnWindowResize(int x, int y, int width, int height);

	bool GetVSync();
	bool SetVSync(bool vsync);

	unsigned int CheckersId() const { return checkers_tex_; };

private:
	void CreateCheckersTexture();


public:
	
	bool vsync = false;

private:

	unsigned int checkers_tex_;


};

#endif // !__ENGINE_CORE_H__