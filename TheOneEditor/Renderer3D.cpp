#include "App.h"

#include "Renderer3D.h"
#include "Window.h"
#include "Gui.h"
#include "PanelScene.h"
#include "SceneManager.h"

#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Component.h"
#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Camera.h"
#include "..\TheOneEngine\EngineCore.h"

#include "glm/gtc/type_ptr.hpp"



Renderer3D::Renderer3D(App* app) : Module(app)
{
}

Renderer3D::~Renderer3D() {}

bool Renderer3D::Awake()
{
    app->engine->Awake();

    return true;
}

bool Renderer3D::Start()
{
    app->engine->Start();

    // Creating Editor Camera GO (Outside hierarchy)
    sceneCamera = std::make_shared<GameObject>("EDITOR CAMERA");
    Transform* camera_transform = (Transform*)sceneCamera.get()->AddComponent<Transform>();
    sceneCamera.get()->AddComponent<Camera>();
    camera_transform->setPosition(vec3f(0, 15, -70));
    camera_transform->updateMatrix();

    FillDefaultShaders(default_shader_);
    default_shader_.CompileShader();

    return true;
}

bool Renderer3D::PreUpdate()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    return true;
}

bool Renderer3D::Update(double dt)
{
    CameraInput(dt);
    app->gui->panelScene->isHovered = false;

    app->engine->Update(dt);

    return true;
}

bool Renderer3D::PostUpdate()
{
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glClearDepth(1.0f);

    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_COLOR_MATERIAL);
    //Camera* cam = sceneCamera->GetComponent<Camera>();
    //mat4f gl_viewmatrix = glm::transpose(cam->getViewMatrix());
    //mat4f gl_projmatrix = glm::transpose(cam->getProjMatrix());

    //// Send proj and view matrix to shader
    //glUniformMatrix4fv(glGetUniformLocation(default_shader_.id, "u_View"), 1, GL_FALSE, glm::value_ptr(gl_viewmatrix));
    //glUniformMatrix4fv(glGetUniformLocation(default_shader_.id, "u_Proj"), 1, GL_FALSE, glm::value_ptr(gl_projmatrix));

    //app->engine->DrawGrid(1000, 10);
    //app->engine->DrawAxis();

    app->sceneManager->RenderScene();

    app->gui->Draw();

    SDL_GL_SwapWindow(app->window->window);

    return true;
}

bool Renderer3D::CleanUp()
{

    return true;
}

void Renderer3D::DrawGameObject(std::shared_ptr<GameObject> object) {
    Mesh* mesh = object->GetComponent<Mesh>();
    if (!mesh)
        return;

    if (mesh->drawAABB)
        mesh->DrawAABB();

    glUseProgram(default_shader_.id);

    Transform* transform = object->GetComponent<Transform>();
    mat4f model = glm::transpose(transform->getMatrix());
    glUniformMatrix4fv(glGetUniformLocation(default_shader_.id, "u_Model"), 1, GL_FALSE, glm::value_ptr(model));

    unsigned int texture = 0;
    if (mesh->mesh.texture) {
        texture = mesh->mesh.texture->Id();
    }

    // Bind stuff and render
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(mesh->VAO());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO());
    glDrawElements(GL_TRIANGLES, mesh->mesh.numIndexs, GL_UNSIGNED_INT, nullptr);


    // Unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void Renderer3D::CreateRay()
{
    //if (app->sceneManager->GetGameObjects().empty())
    //    return;

    ////App->renderer3D->SetCameraToDraw(fake_camera);
    //float2 origin = float2((App->input->GetMousePosition().x - App->ui->panel_scene->posX) / App->ui->panel_scene->width, (App->input->GetMousePosition().y - App->ui->panel_scene->posY) / App->ui->panel_scene->height);

    //origin.x = (origin.x - 0.5F) * 2;
    //origin.y = -(origin.y - 0.5F) * 2;

    //if (origin.x > 1 || origin.x < -1 || origin.y > 1 || origin.y < -1)
    //    return;

    //ray = fake_camera->frustum.UnProjectLineSegment(origin.x, origin.y);

    //std::vector<std::pair<float, GameObject*>> hits;

    //// with octree to static objects
    //CreateObjectsHitMap(&hits, App->objects->octree.root, ray);

    //// without octree for the dynamics
    //std::vector<GameObject*>::iterator item = App->objects->GetRoot(true)->children.begin();

    //for (; item != App->objects->GetRoot(true)->children.end(); ++item)
    //{
    //    if (*item != nullptr && (*item)->IsEnabled()) {
    //        CreateObjectsHitMap(&hits, (*item), ray);
    //    }
    //}
    //// sort by pos
    //std::sort(hits.begin(), hits.end(), ModuleCamera3D::SortByDistance);
    //static bool hit = false;
    //std::vector<std::pair<float, GameObject*>>::iterator it = hits.begin();

    //for (; it != hits.end(); ++it)
    //{
    //    if ((*it).second != nullptr) {
    //        if (TestTrianglesIntersections((*it).second, ray)) {
    //            hit = true;
    //            break;
    //        }
    //    }
    //}

    //if (!hit)
    //    App->objects->DeselectObjects();

    //hit = false;

}



void Renderer3D::FillDefaultShaders(Shader& shader) const {
    // Simple Vertex Shader
    shader.vertex_shader =
        R"(#version 330 core
		layout (location = 0) in vec3 a_Position;
		layout (location = 1) in vec2 a_TexCoord;
		
		uniform vec4 u_Model;
        uniform vec4 u_View;
        uniform vec4 u_Proj;
		
		out vec2 v_TexCoord;
		
		void main() {
			gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0f);
			v_TexCoord = a_TexCoord;
		})";

    // Simple Fragment Shader
    shader.fragment_shader =
        R"(#version 330 core
		uniform sampler2D u_Texture;
		
		in vec2 v_TexCoord;
		out vec4 color;
		
		void main() {
			color = texture(u_Texture, v_TexCoord);
		})";
}

void Renderer3D::CameraInput(double dt)
{
    if (!app->gui->panelScene->isHovered || app->IsPlaying())
        return;

    Camera* camera = sceneCamera.get()->GetComponent<Camera>();
    Transform* transform = sceneCamera.get()->GetComponent<Transform>();

    double speed = 20 * dt;
    if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
        speed = 35 * dt;

    double mouseSensitivity = 18.0 * dt;

    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        /* MOUSE CAMERA MOVEMENT */
        camera->yaw += -app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch += app->input->GetMouseYMotion() * mouseSensitivity;

        camera->setRotation(vec3f(camera->pitch, camera->yaw, 0.0f));

        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
        {
            camera->translate(transform->getForward() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
        {
            camera->translate(-transform->getForward() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
        {
            camera->translate(transform->getRight() * speed);
        }
        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
        {
            camera->translate(-transform->getRight() * speed);
        }
    }
    else
    {
        // Zooming Camera Input
        camera->translate(transform->getForward() * (double)app->input->GetMouseZ());
    }

    // Orbit Object with Alt + LMB
    if (app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
    {
        camera->yaw += -app->input->GetMouseXMotion() * mouseSensitivity;
        camera->pitch += app->input->GetMouseYMotion() * mouseSensitivity;

        camera->setPosition(camera->center);
       
        camera->rotate(vec3f(0.0f, 1.0f, 0.0f), camera->yaw, false);
        camera->rotate(vec3f(1.0f, 0.0f, 0.0f), camera->pitch, true);

        vec3f finalPos;
        finalPos = transform->getPosition() - transform->getForward();
        if (app->sceneManager->GetSelectedGO() != nullptr)
        {
            finalPos = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->getPosition() - (transform->getForward() * 100.0);
        }

        camera->setPosition(finalPos);
    }

    if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && app->sceneManager->GetSelectedGO() != nullptr)
    {
        vec3f targetPos = app->sceneManager->GetSelectedGO().get()->GetComponent<Transform>()->getPosition() - transform->getForward();

        camera->setPosition(targetPos * 100.0f);
    }

    if (transform->isDirty())
        transform->updateMatrix();

    camera->updateCameraVectors();
    camera->updateViewMatrix();
}