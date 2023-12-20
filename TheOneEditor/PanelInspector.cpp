#include "PanelInspector.h"
#include "App.h"
#include "Gui.h"
#include "SceneManager.h"

#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Camera.h"

#include "imgui.h"
#include "imgui_internal.h"


PanelInspector::PanelInspector(PanelType type, std::string name) : Panel(type, name) 
{
    needRefresh_pos = false; 
    needRefresh_rot = false;
    needRefresh_sca = false;

    view_pos = { 0, 0, 0 };
    view_rot = { 0, 0, 0 };
    view_sca = { 0, 0, 0 };
}

PanelInspector::~PanelInspector() {}

bool PanelInspector::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Inspector", &enabled, settingsFlags))
	{
        ImGuiIO& io = ImGui::GetIO();

        ImVec4 clear_color = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

        ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once); //Sets window size only once with ImGuiCond_Once, if fixed size erase it.
        ImGui::Begin("Inspector");

        if (app->sceneManager->GetSelectedGO() != nullptr)
        {
            //ImGui::Checkbox("Active", &gameObjSelected->isActive);
            ImGui::SameLine(); ImGui::Text("GameObject");
            ImGui::SameLine(); ImGui::TextColored({ 0.144f, 0.422f, 0.720f, 1.0f }, app->sceneManager->GetSelectedGO().get()->GetName().c_str());

            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Tag", "Untagged", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }

            ImGui::SameLine();

            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Layer", "Default", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }

            /*Transform Component*/
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::SetItemTooltip("Displays and sets game object transformations");

                Transform* transform = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>();

                view_pos = transform->getPosition();
                view_rot = transform->getEulerAngles();
                view_sca = transform->getScale();


                if (ImGui::BeginTable("", 4))
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    //ImGui::Checkbox("Active", &transform->isActive);
                    ImGui::Text("");
                    ImGui::Text("Position");
                    ImGui::Text("Rotation");
                    ImGui::Text("Scale");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("X");
                    //ImGui::Text(std::to_string(transform->getPosition().x).c_str());
                    //ImGui::Text(std::to_string(transform->getEulerAngles().x).c_str());
                    //ImGui::Text(std::to_string(transform->getScale().x).c_str());

                    if (ImGui::DragFloat("X", &view_pos.x, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_pos = true;
                    }
                    if (ImGui::DragFloat("X", &view_rot.x, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_rot = true;
                    }
                    if (ImGui::DragFloat("X", &view_sca.x, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_sca = true;
                    }

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("Y");
                    //ImGui::Text(std::to_string(transform->getPosition().y).c_str());
                    //ImGui::Text(std::to_string(transform->getEulerAngles().y).c_str());
                    //ImGui::Text(std::to_string(transform->getScale().y).c_str());
                    if (ImGui::DragFloat("Y", &view_pos.y, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_pos = true;
                    }
                    if (ImGui::DragFloat("Y", &view_rot.y, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_rot = true;
                    }
                    if (ImGui::DragFloat("Y", &view_sca.y, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_sca = true;
                    }

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("Z");
                    //ImGui::Text(std::to_string(transform->getPosition().z).c_str());
                    //ImGui::Text(std::to_string(transform->getEulerAngles().z).c_str());
                    //ImGui::Text(std::to_string(transform->getScale().z).c_str());
                    if (ImGui::DragFloat("Z", &view_pos.z, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_pos = true;
                    }
                    if (ImGui::DragFloat("Z", &view_rot.z, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_rot = true;
                    }
                    if (ImGui::DragFloat("Z", &view_sca.z, 0.5F, 0, 0, "%.3f", 1)) {
                        needRefresh_sca = true;
                    }

                    if (needRefresh_pos)
                       transform->setPosition(view_pos /*- transform->getPosition()*/);
                    else if (needRefresh_rot) {
                       //transform->setR(view_rot - original_rot);

                    }
                    else if (needRefresh_sca)
                       transform->setScale(view_sca /*- original_scale*/);

                    ImGui::EndTable();
                }
            }
            //static char buf[5] = "0";
            //ImGui::Text("Position");
            ////ImGui::ItemSize(ImRect(ImVec2(0, 0), ImVec2(5, 5)));
            //ImGui::PushItemWidth(40.0f);
            //ImGui::SameLine();
            //ImGui::InputText("x", buf, IM_ARRAYSIZE(buf));
            //ImGui::SameLine();
            //ImGui::InputText("y", buf, IM_ARRAYSIZE(buf));
            //ImGui::SameLine();
            //ImGui::InputText("z", buf, IM_ARRAYSIZE(buf));

            /*Mesh Component*/
            if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                Mesh* mesh = app->sceneManager->GetSelectedGO().get()->GetComponent<Mesh>();

                if (mesh != nullptr) {
                    ImGui::SetItemTooltip("Displays and sets mesh data");
                    //ImGui::Checkbox("Active", &mesh->isActive);
                    //ImGui::SameLine();  
                    ImGui::Text("Name: ");
                    ImGui::SameLine();  ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, mesh->mesh.meshName.c_str());
                    ImGui::Separator();
                    ImGui::Text("Indexes: ");
                    ImGui::SameLine();  ImGui::Text((std::to_string(mesh->mesh.indexs_buffer_id)).c_str());
                    //ImGui::Text("Normals: ");
                    //ImGui::SameLine();  ImGui::Text(/*std::to_string(mesh->getNumNormals()).c_str()*/"244");
                    ImGui::Text("Vertexs: ");
                    ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.numVerts).c_str());
                    ImGui::Text("Faces: ");
                    ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.numFaces).c_str());
                    //ImGui::Text("Tex coords: ");
                    //ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.getNumTexCoords()).c_str());

                    //if (ImGui::Checkbox("Use Texture", /*&mesh->usingTexture*/true))
                    //{
                    //    //(mesh->usingTexture) ? mesh->texture = gameObjSelected->GetComponent<Texture2D>() : mesh->texture = nullptr;
                    //}
                    ImGui::Checkbox("Active Mesh", &mesh->active);
                    ImGui::Checkbox("Active vertex normals", &mesh->drawNormalsVerts);
                    ImGui::Checkbox("Active face normals", &mesh->drawNormalsFaces);
                    ImGui::Checkbox("Active Wireframe", &mesh->drawWireframe);
                    ImGui::Checkbox("Active AABB", &mesh->drawAABB);
                    ImGui::Checkbox("Active OBB", &mesh->drawOBB); 
                    //ImGui::Checkbox("Active checkboard", &mesh->drawChecker);
                    ImGui::Separator();
                }
                else {
                    ImGui::Text("No meshes found");
                }
            }
            
            /*Texture Component*/
            if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen))
            {
                Texture* tex = app->sceneManager->GetSelectedGO().get()->GetComponent<Texture>();
                
                if (tex != nullptr) {
                    ImGui::SetItemTooltip("Displays and sets texture data");
                    ImGui::Checkbox("Active Texture", &tex->active);
                    ImGui::Text("Name: ");
                    ImGui::SameLine();  ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, (tex->GetName()).c_str());
                    ImGui::Separator();
                    ImGui::Text("Size: ");
                    ImGui::SameLine();  ImGui::Text(std::to_string(tex->width).c_str());
                    ImGui::Text("Height: ");
                    ImGui::SameLine();  ImGui::Text(std::to_string(tex->height).c_str());
                    //ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dpx x %dpx", s->getTexture()->width, s->getTexture()->height);


                    ImGui::Separator();
                }
                else {
                    ImGui::Text("No texture found");
                }
            }
        }

        ImGui::End();
	}	

    ImGui::PopStyleVar();

	return true;
}