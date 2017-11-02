#pragma once
#include "Component.h"
#include "MathGeoLib\Geometry\Frustum.h"
#include "MathGeoLib\Geometry\AABB.h"
#include "MathGeoLib\Math\Rect.h"
#include "Color.h"
#include "Primitive.h"

class RenderTexture;

//MathGeoLib frustum info: http://clb.demon.fi/MathGeoLib/nightly/docs/Frustum_summary.php

class ComponentCamera :
	public Component
{
public:
	ComponentCamera(GameObject* attached_gameobject);
	~ComponentCamera();

	math::Frustum GetFrustum() const;
	bool ContainsGameObjectAABB(AABB& gameobject_bounding_box);
	void UpdatePosition();
	void UpdateProjection();
	float* GetProjectionMatrix() const;
	float* GetViewMatrix();

	void SetFOV(float fov);
	float GetFOV() const;
	void AddLayerToDraw(std::string layer);
	std::string GetLayerToDraw(int index) const;
	void RemoveLayerToDraw(std::string layer);
	void SetBackgroundColor(Color color);
	Color GetBackgroundColor() const;
	void SetNearPlaneDistance(float distance);
	float GetNearPlaneDistance() const;
	void SetFarPlaneDistance(float distance);
	float GetFarPlanceDistance() const;
	float GetAspectRatio() const;
	void SetViewport(Rect viewport);
	Rect GetViewport() const;
	void SetRenderOrder(int position);
	int GetRenderOrder() const;
	void SetTargetTexture(RenderTexture* texture);
	RenderTexture* GetTargetTexture() const;
	RenderTexture* GetViewportTexture() const;

	void Save(Data& data) const;
	void Load(Data& data);

public:
	std::vector<std::string> layers_to_draw;
	Frustum camera_frustum;

private:
	RenderTexture* camera_viewport_texture;
	RenderTexture* camera_target_texture;
	
	
	Color background_color;
	Rect camera_viewport;
	int render_order;
	
};
