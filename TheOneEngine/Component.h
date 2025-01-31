#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Defs.h"
#include "UIDGen.h"

#include <string>
#include <memory>

class GameObject;

enum class ComponentType
{
	Transform,
	Camera,
	Mesh,
    Texture,
    Script,
	Unknown
};

class Component
{
public:

    Component(std::shared_ptr<GameObject> containerGO, ComponentType type);
    virtual ~Component();

    virtual void Enable();
    virtual void Disable();
    virtual void Update() {};

    std::string GetName();
    void CreateNameFromType(ComponentType type);

    ComponentType GetType() const;
    std::shared_ptr<GameObject> GetContainerGO() const;

    bool IsEnabled() const;

    //virtual void CreateInspectorNode() = 0;
    //virtual void CreateInspectorNode();
    virtual void DrawComponent() {};

    void CreateUID() { UID = UIDGen::GenerateUID(); }
    uint32 GetUID() { return UID; }

    virtual json SaveComponent() = 0;
    virtual void LoadComponent(const json& transformJSON) = 0;

    // Is this component only allowed one instance per GameObject?
    bool IsUnique() const { return unique; };

protected:
    std::weak_ptr<GameObject> containerGO;
    ComponentType type;
    std::string name;
    bool unique = true;

    uint32_t UID;
public:
    bool enabled;
};

#endif // !__COMPONENT_H__