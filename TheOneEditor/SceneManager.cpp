#include "App.h"
#include "SceneManager.h"
#include "Log.h"

#include <fstream>
#include <filesystem>
#include "../TheOneEngine/ComponentScript.h"
#include "ModuleScripting.h"
#include "DemoFunctions.h"
#include "Renderer3D.h"

#include "../TheOneEngine/par_shapes.h"
#include "../TheOneEngine/ScriptData.h"

namespace fs = std::filesystem;

SceneManager::SceneManager(App* app) : Module(app), selectedGameObject(0)
{
    meshLoader = new MeshLoader();
    rootSceneGO = std::make_shared<GameObject>("Scene");;
}

SceneManager::~SceneManager()
{
    delete meshLoader;
}

bool SceneManager::Awake()
{
    std::filesystem::create_directories("Library/");
    return true;
}

bool SceneManager::Start()
{
    // hekbas testing creation of GO
    
    //CreateEmptyGO();
    //CreateCube();
    //CreateSphere();
    
    //CreateMeshGO("Assets\\Meshes\\baker_house.fbx");
    CreateMeshGO("Assets\\Meshes\\street.fbx");
    //demo = CreateMeshGO("Assets\\Meshes\\Cadillac_CT4_V_2022.fbx");
    //ComponentScript* demo_script = demo->AddScriptComponent("Assets\\Scripts\\DemoTankMovement.lua");
    //app->scripting->CreateScript(demo_script);
    demo = Demo::CreateTank();
    Demo::CreateBullet();

    rotationAngle = 0.0f;
    rotationSpeed = 30.0f;

    std::shared_ptr<GameObject> gameCam = CreateCameraGO("Game Camera");
    gameCam.get()->GetComponent<Camera>()->setPosition({ -10, 8, 0 });

    return true;
}

bool SceneManager::PreUpdate()
{
    for (auto child : rootSceneGO->children)
        child->UpdateTransform();
    return true;
}

bool SceneManager::Update(double dt)
{
    //Save Scene
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
    {
        SaveScene();
    }
    //Load Scene
    if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
    {
        std::string filename = "Assets/Scenes/scene.toe";
        LoadScene(filename);
    }


    if (app->IsPlaying()) {
        //demo->GetComponent<Transform>()->setRotation({ 0, 1, 0 }, rotationAngle);
        //
        //rotationAngle += rotationSpeed * dt;
        //
        //if (rotationAngle >= 360.0f)
        //    rotationAngle -= 360.0f;
    }

    if (app->state == GameState::NONE) {
        demo->GetComponent<Transform>()->setRotation({ 0, 0, 0 });
        demo->GetComponent<Transform>()->setPosition({ 0, 1, 0 });
        DeleteInstancedObjects();
        rotationAngle = 0.0;
    }

    return true;
}

bool SceneManager::PostUpdate()
{
    //RecurseDrawChildren(rootSceneGO);

    return true;
}

bool SceneManager::CleanUp()
{
    gameobjects.clear();
    return true;
}

std::string SceneManager::GenerateUniqueName(const std::string& baseName)
{
    std::string uniqueName = baseName;
    int counter = 1;

    while (std::any_of(
        rootSceneGO.get()->children.begin(), rootSceneGO.get()->children.end(),
        [&uniqueName](const std::shared_ptr<GameObject>& obj)
        { return obj.get()->GetName() == uniqueName; }))
    {
        uniqueName = baseName + "(" + std::to_string(counter) + ")";
        ++counter;
    }

    return uniqueName;
}

std::shared_ptr<GameObject> SceneManager::CreateEmptyGO(std::string name)
{
    std::shared_ptr<GameObject> emptyGO = std::make_shared<GameObject>(name);
    emptyGO.get()->AddComponent<Transform>();

    emptyGO.get()->parent = rootSceneGO.get()->weak_from_this();

    rootSceneGO.get()->children.emplace_back(emptyGO);
    gameobjects[emptyGO->GetUID()] = emptyGO;

    return emptyGO;
}

std::shared_ptr<GameObject> SceneManager::CreateCameraGO(std::string name)
{
    std::shared_ptr<GameObject> cameraGO = std::make_shared<GameObject>(name);
    cameraGO.get()->AddComponent<Transform>();
    cameraGO.get()->AddComponent<Camera>();

    cameraGO.get()->parent = rootSceneGO.get()->weak_from_this();

    rootSceneGO.get()->children.emplace_back(cameraGO);

    return cameraGO;
}

std::shared_ptr<GameObject> SceneManager::CreateMeshGO(std::string path)
{
    std::vector<MeshBufferedData> meshes = meshLoader->LoadMesh(path);
    std::vector<std::shared_ptr<Texture>> textures = meshLoader->LoadTexture(path);

    if (!meshes.empty())
    {
        std::string name = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);

        //Take name before editing for meshData lookUp
        std::string folderName = "Library/Meshes/" + name + "/";

        name = GenerateUniqueName(name);

        // Create emptyGO parent if meshes >1
        bool isSingleMesh = meshes.size() > 1 ? false : true;
        std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
        if (!isSingleMesh) emptyParent.get()->SetName(name);

        std::vector<std::string> fileNames;

        uint fileCount = 0;

        for (const auto& entry : fs::directory_iterator(folderName))
        {
            if (fs::is_regular_file(entry))
            {
                std::string path = entry.path().filename().string();
                fileNames.push_back(entry.path().string());
                fileCount++;
            }
        }

        for (auto& mesh : meshes)
        {
            std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh.meshName);
            gameobjects[meshGO->GetUID()] = meshGO;
            meshGO.get()->AddComponent<Transform>();
            Mesh* go_mesh = (Mesh*) meshGO.get()->AddComponent<Mesh>();
            //meshGO.get()->AddComponent<Texture>(); // hekbas: must implement

            go_mesh->mesh = mesh;
            go_mesh->mesh.texture = textures[mesh.materialIndex];
            //meshGO.get()->GetComponent<Texture>() = &meshGO.get()->GetComponent<Mesh>()->mesh.texture;

            //Load MeshData from custom files
            for (const auto& file : fileNames)
            {
                std::string fileName = file.substr(file.find_last_of("\\/") + 1, file.find_last_of('.') - file.find_last_of("\\/") - 1);
                if (fileName == mesh.meshName)
                {
                    MeshData mData = meshLoader->deserializeMeshData(file);

                    go_mesh->meshData = mData;
                    go_mesh->path = file;
                }
                
            }

            // hekbas: need to set Transform?

            go_mesh->GenerateAABB();
            go_mesh->GenerateShaderObjects();

            if (isSingleMesh)
            {
                meshGO.get()->parent = rootSceneGO;
                rootSceneGO.get()->children.push_back(meshGO);
                return meshGO;
            }
            else
            {
                meshGO.get()->parent = emptyParent;
                emptyParent.get()->children.push_back(meshGO);
            }
        }

        return emptyParent;

       /* if (!textures.empty())
       {
            if (meshes.size() == 1)
            {
                ComponentTexture* texture = (ComponentTexture*)GetComponent(ComponentType::TEXTURE);
                textures->setTexture((*meshes.begin())->texture);
                defaultTexture = texture->getTexture()->path;
            }
            else
            {
                for (auto i = meshes.begin(); i != meshes.end(); ++i)
                {
                    ComponentTexture* texturePart = (ComponentTexture*)GOPart->GetComponent(ComponentType::TEXTURE);
                    texturePart->setTexture((*i)->texture);
                    defaultTexture = texturePart->getTexture()->path;
                }
            }
        }*/
    }

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateExistingMeshGO(std::string path)
{
    std::string fbxName = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);

    std::string folderName = "Library/Meshes/" + fbxName + "/";

    std::vector<std::string> fileNames;

    uint fileCount = 0;

    if (fs::is_directory(folderName))
    {
        for (const auto& entry : fs::directory_iterator(folderName)) {
            if (fs::is_regular_file(entry)) {
                std::string path = entry.path().filename().string();
                LOG(LogType::LOG_WARNING, "- %s is in", path.data());
                fileNames.push_back(entry.path().string());
                fileCount++;
            }
        }
    }

    if (fileCount < 1)
    {
        CreateMeshGO(path);
    }
    else
    {
        std::string name = fbxName;
        name = GenerateUniqueName(name);

        // Create emptyGO parent if meshes >1
        bool isSingleMesh = fileCount > 1 ? false : true;
        std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
        if (!isSingleMesh) emptyParent.get()->SetName(name);

        for (const auto& file : fileNames)
        {
            MeshData mData = meshLoader->deserializeMeshData(file);

            meshLoader->BufferData(mData);

            std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mData.meshName);
            gameobjects[meshGO->GetUID()] = meshGO;
            meshGO.get()->AddComponent<Transform>();
            Mesh* go_mesh = (Mesh*)meshGO.get()->AddComponent<Mesh>();
            //meshGO.get()->AddComponent<Texture>(); // hekbas: must implement

            go_mesh->meshData = mData;
            go_mesh->mesh = meshLoader->GetBufferData();
            go_mesh->path = file;
            go_mesh->GenerateShaderObjects();
            //meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex]; //Implement texture deserialization
            // hekbas: need to set Transform?

            go_mesh->GenerateAABB();

            if (isSingleMesh)
            {
                meshGO.get()->parent = rootSceneGO;
                rootSceneGO.get()->children.push_back(meshGO);
            }
            else
            {
                meshGO.get()->parent = emptyParent;
                emptyParent.get()->children.push_back(meshGO);
            }

        }
    }
    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::CreateCube()
{
    std::shared_ptr<GameObject> cubeGO = std::make_shared<GameObject>("Cube");
    gameobjects[cubeGO->GetUID()] = cubeGO;
    cubeGO.get()->AddComponent<Transform>();
    Mesh* component_mesh = (Mesh*)cubeGO.get()->AddComponent<Mesh>();

    cubeGO.get()->parent = rootSceneGO.get()->weak_from_this();

    // We create the cube using planes and rotating and joining them
    par_shapes_mesh* mesh = par_shapes_create_plane(1, 1);
    par_shapes_mesh* top = par_shapes_create_plane(1, 1);
    par_shapes_mesh* bottom = par_shapes_create_plane(1, 1);
    par_shapes_mesh* back = par_shapes_create_plane(1, 1);
    par_shapes_mesh* left = par_shapes_create_plane(1, 1);
    par_shapes_mesh* right = par_shapes_create_plane(1, 1);

    float axisX[3] = { 1, 0, 0 };
    float axisY[3] = { 0, 1, 0 };

    par_shapes_translate(mesh, -0.5f, -0.5f, 0.5f);

    par_shapes_rotate(top, -float(PAR_PI * 0.5), axisX);
    par_shapes_translate(top, -0.5f, 0.5f, 0.5f);

    par_shapes_rotate(bottom, float(PAR_PI * 0.5), axisX);
    par_shapes_translate(bottom, -0.5f, -0.5f, -0.5f);

    par_shapes_rotate(back, float(PAR_PI), axisX);
    par_shapes_translate(back, -0.5f, 0.5f, -0.5f);

    par_shapes_rotate(left, float(-PAR_PI * 0.5), axisY);
    par_shapes_translate(left, -0.5f, -0.5f, -0.5f);

    par_shapes_rotate(right, float(PAR_PI * 0.5), axisY);
    par_shapes_translate(right, 0.5f, -0.5f, 0.5f);

    par_shapes_merge_and_free(mesh, top);
    par_shapes_merge_and_free(mesh, bottom);
    par_shapes_merge_and_free(mesh, back);
    par_shapes_merge_and_free(mesh, left);
    par_shapes_merge_and_free(mesh, right);

    if (mesh) {
        MeshBufferedData buffer_data;
        MeshData data;
        meshLoader->LoadMeshFromPar(mesh, "Cube", data, buffer_data);
        component_mesh->mesh = buffer_data;
        component_mesh->meshData = data;
        component_mesh->GenerateShaderObjects();
    }

    rootSceneGO.get()->children.emplace_back(cubeGO);

    return cubeGO;
}

std::shared_ptr<GameObject> SceneManager::CreateSphere(float radius, int slices, int slacks)
{
    std::shared_ptr<GameObject> sphereGO = std::make_shared<GameObject>("Sphere");
    gameobjects[sphereGO->GetUID()] = sphereGO;
    sphereGO.get()->AddComponent<Transform>();
    Mesh* component_mesh = (Mesh*)sphereGO.get()->AddComponent<Mesh>();

    sphereGO.get()->parent = rootSceneGO.get()->weak_from_this();

    par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(slices, slacks);

    if (mesh) {
        par_shapes_scale(mesh, radius / 2, radius / 2, radius / 2);
        MeshBufferedData buffer_data;
        MeshData data;
        meshLoader->LoadMeshFromPar(mesh, "Sphere", data, buffer_data);
        component_mesh->mesh = buffer_data;
        component_mesh->meshData = data;
        component_mesh->GenerateShaderObjects();
    }

    rootSceneGO.get()->children.emplace_back(sphereGO);
    
    return sphereGO;
}

std::shared_ptr<GameObject> SceneManager::CreateMF()
{
 

    return CreateMeshGO("Assets/Meshes/mf.fbx");;
}

uint SceneManager::GetNumberGO() const
{
    return static_cast<uint>(rootSceneGO.get()->children.size());
}

std::vector<std::shared_ptr<GameObject>> SceneManager::GetGameObjects()
{
    return rootSceneGO.get()->children;
}

void SceneManager::SetSelectedGO(std::shared_ptr<GameObject> gameObj)
{
    selectedGameObject = gameObj;
}

std::shared_ptr<GameObject> SceneManager::GetSelectedGO() const
{
    return selectedGameObject;
}

std::shared_ptr<GameObject> SceneManager::GetRootSceneGO() const
{
    return rootSceneGO;
}

std::shared_ptr<GameObject> SceneManager::FindGOByUID(uint32_t _UID) const
{
    auto it = gameobjects.find(_UID);
    if (it != gameobjects.end())
        return it->second;
    else
        return nullptr;
}

std::shared_ptr<GameObject> SceneManager::FindGOByName(std::string name) const {
    for (auto it = gameobjects.begin(); it != gameobjects.end(); ++it) {
        if (it->second->GetName() == name)
            return it->second;
    }

    return nullptr;
}

std::shared_ptr<GameObject> SceneManager::InstantiateGameObject(unsigned int UID) {
    auto location = gameobjects.find(UID);
    if (location == gameobjects.end())
        return std::shared_ptr<GameObject>();

    std::shared_ptr<GameObject> original = location->second;
    std::shared_ptr<GameObject> ret = std::make_shared<GameObject>(original->GetName() + std::to_string(instance_counter));
    rootSceneGO->children.push_back(ret);
    ret->parent = rootSceneGO;
    gameobjects[ret->GetUID()] = ret;
    ret->AddComponent<Transform>();

    std::vector<Component*> original_components = original->GetAllComponents();
    Mesh* ret_mesh = nullptr;
    ComponentScript* ret_script = nullptr;
    for (Component* component : original_components) {
        switch (component->GetType()) {
        case ComponentType::Mesh:
            ret_mesh = (Mesh*)ret->AddComponent<Mesh>();
            ret_mesh->mesh = ((Mesh*)component)->mesh;
            ret_mesh->meshData = ((Mesh*)component)->meshData;
            ret_mesh->GenerateShaderObjects();
            ret_mesh->GenerateAABB();
            break;
        case ComponentType::Script:
            ret_script = ret->AddScriptComponent("");
            ret_script->data->path = ((ComponentScript*)component)->data->path;
            ret_script->data->name = ((ComponentScript*)component)->data->name;
            app->scripting->CreateScript(ret_script);
            break;
        }
    }

    instances.push_back(ret);
    ++instance_counter;
    return ret;
}

void SceneManager::RenderScene() {
    RecurseDrawChildren(rootSceneGO);
}

void SceneManager::DestroyGameObject(unsigned int UID) {
    auto object = gameobjects.find(UID);
    if (object == gameobjects.end())
        return;
    
    std::shared_ptr<GameObject> to_delete = object->second;
    gameobjects.erase(object);
    std::shared_ptr<GameObject> parent = to_delete->parent.lock();
    for (auto child = parent->children.begin(); child != parent->children.end(); ++child) {
        if ((*child) == to_delete) {
            parent->children.erase(child);
            break;
        }
    }
}

void SceneManager::SaveScene()
{
    fs::path filename = fs::path(ASSETS_PATH) / "Scenes" / "scene.toe";
    //string filename = "Assets/Scenes/";
    fs::path folderName = fs::path(ASSETS_PATH) / "Scenes";
    fs::create_directories(folderName);

    json sceneJSON;

    json gameObjectsJSON;
    /*Save all gameobjects*/
    for (const auto& go : GetGameObjects())
    {
        gameObjectsJSON.push_back(go.get()->SaveGameObject());
    }

    sceneJSON["GameObjects"] = gameObjectsJSON;

    std::ofstream(filename) << sceneJSON.dump(2);
    LOG(LogType::LOG_OK, "SAVE SUCCESFUL");
}

void SceneManager::LoadScene(const std::string& filename)
{
    // Check if the scene file exists
    if (!fs::exists(filename))
    {
        LOG(LogType::LOG_ERROR, "Scene file does not exist: {}", filename.data());
        return;
    }

    // Read the scene JSON from the file
    std::ifstream file(filename);
    if (!file.is_open())
    {
        LOG(LogType::LOG_ERROR, "Failed to open scene file: {}", filename.data());
        return;
    }

    json sceneJSON;
    try
    {
        file >> sceneJSON;
    }
    catch (const json::parse_error& e)
    {
        LOG(LogType::LOG_ERROR, "Failed to parse scene JSON: {}", e.what());
        return;
    }

    // Close the file
    file.close();

    rootSceneGO.get()->children.clear();

    // Load game objects from the JSON data
    if (sceneJSON.contains("GameObjects"))
    {
        const json& gameObjectsJSON = sceneJSON["GameObjects"];

        for (const auto& gameObjectJSON : gameObjectsJSON)
        {
            // Create a new game object
            auto newGameObject = CreateEmptyGO();
            auto it = gameobjects.find(newGameObject->GetUID());
            gameobjects.erase(it);

            // Load the game object from JSON
            newGameObject->LoadGameObject(gameObjectJSON);
            gameobjects[newGameObject->GetUID()] = newGameObject; // We load it with the new UID
            // FIXME: Due to inheritance we'd have to go through all children and grab their scripts too
            // for now we just don't load scripts automatically
            // Load Scripts
            //std::vector<ComponentScript*> script_components = newGameObject->GetAllComponents<ComponentScript>();
            //for (auto& script : script_components) {
            //    app->scripting->CreateScript(script);
            //}
            //script_components.clear();

        }

        LOG(LogType::LOG_OK, "LOAD SUCCESSFUL");
    }
    else
    {
        LOG(LogType::LOG_ERROR, "Scene file does not contain GameObjects information");
    }

}

void SceneManager::RecurseDrawChildren(std::shared_ptr<GameObject> parentGO)
{
    for (const auto gameObject : parentGO.get()->children)
    {
        gameObject->Draw();
        RecurseDrawChildren(gameObject);
    }
}

void SceneManager::DeleteInstancedObjects() {
    for (auto object : instances) {
        std::shared_ptr<GameObject> to_delete = object.lock();
        if (!to_delete)
            continue;
        DestroyGameObject(to_delete->GetUID());
    }

    instances.clear();
    instance_counter = 0;
}
