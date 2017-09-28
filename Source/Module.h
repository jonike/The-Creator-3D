#pragma once
#include "Globals.h"
#include "Timer.h"
#include <string>
class Application;
struct PhysBody3D;
class Data;

class Module
{
private :
	bool enabled;
	std::string name;
public:
	Timer ms_timer;
	Application* App;

	Module(Application* parent, bool start_enabled = true, std::string name = "") : App(parent)
	{}

	virtual ~Module()
	{}

	virtual bool Init(Data* editor_config = nullptr) 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void SaveData(Data* data)
	{}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}
};