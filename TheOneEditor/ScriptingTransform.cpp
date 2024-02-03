#include "ScriptingTransform.h"
#include "SceneManager.h"
#include "App.h"
#include "../TheOneEngine/luadefs.h"
#include "../TheOneEngine/Transform.h"

luabridge::LuaRef ScriptingTransform::GetPosition(unsigned int go_UUID, lua_State* lua_state) const{
	vec3 position = vec3(0.0f);
	std::shared_ptr<GameObject> go = app->sceneManager->FindGOByUID(go_UUID);
	if (go) {
		Transform* m_transform = go->GetComponent<Transform>();
		position = m_transform->getPosition();
	} else {
		LOG(LogType::LOG_WARNING, "GameObject with UID %d does not exist.", go_UUID);
	}

	luabridge::LuaRef table = luabridge::newTable(lua_state);
	table.append(position.x);
	table.append(position.y);
	table.append(position.z);

	return table;
}

void ScriptingTransform::SetPosition(unsigned int go_UUID, float x, float y, float z) {
	std::shared_ptr<GameObject> go = app->sceneManager->FindGOByUID(go_UUID);
	if (go) {
		Transform* m_transform = go->GetComponent<Transform>();
		m_transform->setPosition(vec3(x, y, z));
	}
	else {
		LOG(LogType::LOG_WARNING, "GameObject with UID %d does not exist.", go_UUID);
	}
}

void ScriptingTransform::Translate(unsigned int go_UUID, float x, float y, float z) {
	std::shared_ptr<GameObject> go = app->sceneManager->FindGOByUID(go_UUID);
	if (go) {
		Transform* m_transform = go->GetComponent<Transform>();
		m_transform->translate(vec3(x, y, z));
	}
	else {
		LOG(LogType::LOG_WARNING, "GameObject with UID %d does not exist.", go_UUID);
	}
}

luabridge::LuaRef ScriptingTransform::GetRotation(unsigned int go_UUID, lua_State* lua_state) const{
	vec3 rotation = vec3(0.0f);
	std::shared_ptr<GameObject> go = app->sceneManager->FindGOByUID(go_UUID);
	if (go) {
		Transform* m_transform = go->GetComponent<Transform>();
		rotation = m_transform->getEulerAngles();
	}
	else {
		LOG(LogType::LOG_WARNING, "GameObject with UID %d does not exist.", go_UUID);
	}

	luabridge::LuaRef table = luabridge::newTable(lua_state);
	table.append(rotation.x);
	table.append(rotation.y);
	table.append(rotation.z);

	return table;
}

void ScriptingTransform::SetRotation(unsigned int go_UUID, float x, float y, float z) {
	std::shared_ptr<GameObject> go = app->sceneManager->FindGOByUID(go_UUID);
	if (go) {
		Transform* m_transform = go->GetComponent<Transform>();
		m_transform->setRotation(vec3(x, y, z));
	}
	else {
		LOG(LogType::LOG_WARNING, "GameObject with UID %d does not exist.", go_UUID);
	}
}

void ScriptingTransform::Rotate(unsigned int go_UUID, float x, float y, float z) {
	std::shared_ptr<GameObject> go = app->sceneManager->FindGOByUID(go_UUID);
	if (go) {
		Transform* m_transform = go->GetComponent<Transform>();
		m_transform->rotate(vec3(x, y, z));
	}
	else {
		LOG(LogType::LOG_WARNING, "GameObject with UID %d does not exist.", go_UUID);
	}
}

void ScriptingTransform::RotateOnAxis(unsigned int go_UUID, float axis_x, float axis_y, float axis_z, float angle) {
	std::shared_ptr<GameObject> go = app->sceneManager->FindGOByUID(go_UUID);
	if (go) {
		Transform* m_transform = go->GetComponent<Transform>();
		m_transform->rotate(vec3(axis_x, axis_y, axis_z), angle);
	}
	else {
		LOG(LogType::LOG_WARNING, "GameObject with UID %d does not exist.", go_UUID);
	}
}
