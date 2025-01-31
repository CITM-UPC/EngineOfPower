#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#pragma once

#include "Module.h"
#include "..\TheOneEngine\Defs.h"
#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"
#include "..\TheOneEngine\MeshLoader.h"
#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Camera.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Texture.h"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>

#define NOT_SPECIFIED ""

class SceneManager : public Module
{
public:
    SceneManager(App* app);

    virtual ~SceneManager();

    bool Awake();
    bool Start();

    bool PreUpdate();
    bool Update(double dt);
    bool PostUpdate();

    bool CleanUp();

    std::string GenerateUniqueName(const std::string& baseName);

    // Basic 3D
    std::shared_ptr<GameObject> CreateEmptyGO(std::string name = "Empty GameObject");
    std::shared_ptr<GameObject> CreateCameraGO(std::string name);
    std::shared_ptr<GameObject> CreateMeshGO(std::string path);
    std::shared_ptr<GameObject> CreateExistingMeshGO(std::string fbxName);
    std::shared_ptr<GameObject> CreateCube();
    std::shared_ptr<GameObject> CreateSphere(float radius = 1.0f, int slices = 25, int slacks = 25);
    std::shared_ptr<GameObject> CreateMF();

    // Get/Set
    uint GetNumberGO() const;
    std::vector<std::shared_ptr<GameObject>>GetGameObjects();
    
    std::shared_ptr<GameObject> GetSelectedGO() const;
    void SetSelectedGO(std::shared_ptr<GameObject> gameObj);

    std::shared_ptr<GameObject> GetRootSceneGO() const;

    std::shared_ptr<GameObject> FindGOByUID(uint32_t _UID) const;
    std::shared_ptr<GameObject> FindGOByName(std::string name) const;

    static SceneManager& GetInstance() {
        static SceneManager instance; // Static instance of the class
        return instance;
    }

    std::shared_ptr<GameObject> InstantiateGameObject(unsigned int UID);

    void RenderScene();

    // GameObject Management
    void DestroyGameObject(unsigned int UID);

    /*SCENE SERIALIZATION*/
    void SaveScene();
    void LoadScene(const std::string& filename);

    //MeshLoader GetMeshLoader();

public:
    std::unordered_map <unsigned int, std::shared_ptr<GameObject>> gameobjects;

private:
    // Private constructor to enforce singleton pattern
    SceneManager() {}

    void RecurseDrawChildren(std::shared_ptr<GameObject> parentGO);
    void DeleteInstancedObjects();

private:
    //uint selectedGameObject;
    std::shared_ptr<GameObject> rootSceneGO;
    std::shared_ptr<GameObject> selectedGameObject;
    std::vector<std::weak_ptr<GameObject>> instances;
    MeshLoader* meshLoader;

    std::shared_ptr<GameObject> demo;
    double rotationAngle;
    double rotationSpeed;
    uint instance_counter;

};

#endif // !__SCENE_MANAGER_H__