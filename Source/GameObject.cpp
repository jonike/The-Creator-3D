#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

GameObject::GameObject(GameObject* parent)
{
	active = true;
	this->parent = nullptr;
	if (parent != nullptr)
	{
		SetParent(parent);
	}
	else
	{
		is_root = true;
	}
	tag = "Default";
	layer = "Default";
	name = "New GameObject";
	is_on_destroy = false;
	is_selected = false;
	is_static = false;
	AddComponent(Component::Transform);

	uuid = App->RandomNumber().Int();
}

GameObject::~GameObject()
{
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); ++it) {
		RELEASE(*it);
	}

	for (std::list<GameObject*>::iterator it = childs.begin(); it != childs.end(); ++it) {
		RELEASE(*it);
	}
}

Component * GameObject::AddComponent(Component::ComponentType component_type)
{
	Component* component = nullptr;

	switch (component_type)
	{
	case Component::Transform:
		components_list.push_back(component = new ComponentTransform(this));
		break;
	case Component::Camera:
		components_list.push_back(component = new ComponentCamera(this));
		break;
	case Component::RigidBody:
		break;
	case Component::MeshRenderer:
		components_list.push_back(component = new ComponentMeshRenderer(this));
		break;
	case Component::BoxCollider:
		break;
	case Component::CircleCollider:
		break;
	case Component::AudioSource:
		break;
	case Component::Animaton:
		break;
	case Component::Script:
		break;
	case Component::ParticleSystem:
		break;
	default:
		break;
	}

	return component;
}

Component * GameObject::GetComponent(Component::ComponentType component_type)
{
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetType() == component_type) {
			return (*it);
		}
	}
	return nullptr;
}

Component * GameObject::GetComponent(std::string component_type)
{
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end(); it++) {
		if ((*it)->GetName() == component_type) {
			return (*it);
		}
	}
	return nullptr;
}

void GameObject::DestroyComponent(Component::ComponentType component)
{
	for (std::list<Component*>::iterator it = components_list.begin(); it != components_list.end();) {
		if ((*it)->GetType() == component) {
			RELEASE(*it);
			it = components_list.erase(it);
		}
		else {
			it++;
		}
	}
}

void GameObject::AddChild(GameObject * gameobject)
{
	childs.push_back(gameobject);
}

bool GameObject::IsActive() const
{
	return active;
}

void GameObject::SetActive(bool active)
{
	this->active = active;
	if (is_static)
	{
		if (active)
		{
			App->scene->InsertGoInOctree(this);
		}
		else
		{
			App->scene->EraseGoInOctree(this);
		}
	}
}

void GameObject::SetStatic(bool is_static)
{
	this->is_static = is_static;
	if (active)
	{
		if (is_static)
		{
			App->scene->InsertGoInOctree(this);
		}
		else
		{
			App->scene->EraseGoInOctree(this);
		}
	}
}

bool GameObject::IsStatic() const
{
	return is_static;
}

bool GameObject::IsSelected() const
{
	return is_selected;
}

void GameObject::SetSelected(bool selected)
{
	is_selected = selected;
}

bool GameObject::IsRoot() const
{
	return is_root;
}

void GameObject::SetRoot(bool root)
{
	is_root = root;
}

GameObject * GameObject::GetParent() const
{
	return parent;
}

void GameObject::SetParent(GameObject * parent)
{
	if (this->parent == parent) {
		return;
	}

	if (this->parent != nullptr) {
		this->parent->childs.remove(this);
	}

	this->parent = parent;
	this->parent->childs.push_back(this);
	is_root = false;
	App->scene->root_gameobjects.remove(this);
}

void GameObject::SetName(std::string name)
{
	this->name = name;
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetTag(std::string tag)
{
	this->tag = tag;
}

std::string GameObject::GetTag() const
{
	return tag;
}

void GameObject::SetLayer(std::string layer)
{
	this->layer = layer;
}

std::string GameObject::GetLayer() const
{
	return layer;
}

void GameObject::UpdateBoundingBox()
{
	ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)GetComponent(Component::MeshRenderer);
	if (mesh_renderer != nullptr)
	{
		mesh_renderer->UpdateBoundingBox();
		if (is_static && active)
		{
			App->scene->EraseGoInOctree(this);
			App->scene->InsertGoInOctree(this);
		}
	}
}

math::float4x4 GameObject::GetGlobalTransfomMatrix()
{
	ComponentTransform* transform = (ComponentTransform*)GetComponent(Component::Transform);
	return transform->GetMatrix();
}

const float * GameObject::GetOpenGLMatrix()
{
	ComponentTransform* transform = (ComponentTransform*)GetComponent(Component::Transform);
	return transform->GetOpenGLMatrix();
}

void GameObject::UpdateGlobalMatrix()
{
	ComponentTransform* transform = (ComponentTransform*)GetComponent(Component::Transform);
	transform->UpdateGlobalMatrix();
}

void GameObject::UpdateCamera()
{
	ComponentCamera* camera = (ComponentCamera*)GetComponent(Component::Camera);
	if (camera != nullptr)
	{
		camera->UpdatePosition();
	}
}

void GameObject::Destroy()
{
	App->scene->AddGameObjectToDestroy(this);
}

void GameObject::OnDestroy()
{
	is_on_destroy = true;

	std::list<GameObject*>::iterator it = find(App->scene->selected_gameobjects.begin(), App->scene->selected_gameobjects.end(), this);
	if (it != App->scene->selected_gameobjects.end()) {
		App->scene->selected_gameobjects.erase(it);
		is_selected = false;
	}

	std::list<GameObject*>::iterator it2 = find(App->scene->scene_gameobjects.begin(), App->scene->scene_gameobjects.end(), this);
	if (it2 != App->scene->scene_gameobjects.end()) {
		App->scene->scene_gameobjects.erase(it2);
	}
	
	for (std::list<GameObject*>::iterator it = childs.begin(); it != childs.end();) {
		if (*it != nullptr) {
			(*it)->OnDestroy();
			CONSOLE_DEBUG("GameObject Destroyed: %s", (*it)->GetName().c_str());
			RELEASE(*it);
			it = childs.erase(it);
		}
	}

	if (parent != nullptr && !parent->is_on_destroy) {
		parent->childs.remove(this);
	}
	
}

UID GameObject::GetUID() const
{
	return uuid;
}

