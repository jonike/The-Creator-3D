#include "ModuleImport.h"
#include "GameObject.h"
#include "ComponentMeshRenderer.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"
#include "OpenGL.h"
#include "Mesh.h"

#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/cimport.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "Devil/include/il.h"
#include "Devil/include/ilut.h"
#pragma comment ( lib, "Devil/libx86/DevIL.lib" )
#pragma comment ( lib, "Devil/libx86/ILU.lib" )
#pragma comment ( lib, "Devil/libx86/ILUT.lib" )



ModuleImport::ModuleImport(Application * app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Importer";

#if _DEBUG
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback("bla", stream.user);
	aiAttachLogStream(&stream);
#endif

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

ModuleImport::~ModuleImport()
{
	
}

bool ModuleImport::CleanUp()
{
	aiDetachAllLogStreams();
	ilShutDown();
	return true;
}

void ModuleImport::LoadFile(std::string path)
{
	if (path.find(".fbx") != std::string::npos || path.find(".FBX") != std::string::npos)
	{
		LoadMesh(path.c_str());
	}
	else if(path.find(".png") != std::string::npos || path.find(".jpg") != std::string::npos)
	{
		LoadTexture(path.c_str(), true); //<-- Right now, we will use the textures only to attach them to gameobjects (Assigment1)
	}
}

bool ModuleImport::LoadMesh(const char* path)
{
	bool ret = true;

	if (!App->scene->root_gameobjects.empty())
	{
		App->scene->AddGameObjectToDestroy(App->scene->root_gameobjects.front());
	}

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		aiNode* root_node = scene->mRootNode;
		LoadMeshNode(nullptr, root_node, scene, path);

		aiReleaseImport(scene);
	}
	else
		CONSOLE_LOG("Error loading scene %s", path);

	return ret;
}

bool ModuleImport::LoadMeshNode(GameObject * parent, aiNode * node, const aiScene * scene, const char* path)
{
	bool ret = true;

	GameObject* root = nullptr;

	if (node->mNumMeshes < 1)
	{
		std::string s_node_name(node->mName.C_Str());
		if (s_node_name.find("$AssimpFbx$_PreRotation") == std::string::npos && s_node_name.find("$AssimpFbx$_Rotation") == std::string::npos &&
			s_node_name.find("$AssimpFbx$_PostRotation") == std::string::npos && s_node_name.find("$AssimpFbx$_Scaling") == std::string::npos &&
			s_node_name.find("$AssimpFbx$_Translation") == std::string::npos)
		{
			std::string root_go_name;
			(s_node_name.find("RootNode") != std::string::npos) ? root_go_name = GetFileName(path) : root_go_name = s_node_name;
			root = new GameObject(parent);
			root->SetName(root_go_name);
			App->scene->AddGameObjectToScene(root);
		}
		else {
			root = parent;
		}
	}
	else
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
			Mesh* mesh = new Mesh();
			mesh->name = (std::string)ai_mesh->mName.C_Str();
			mesh->num_vertices = ai_mesh->mNumVertices;
			mesh->vertices = new float[mesh->num_vertices * 3];
			memcpy(mesh->vertices, ai_mesh->mVertices, sizeof(float) * mesh->num_vertices * 3);
			CONSOLE_LOG("New mesh with %d vertices", mesh->num_vertices);

			if (ai_mesh->HasFaces())
			{
				mesh->num_indices = ai_mesh->mNumFaces * 3;
				mesh->indices = new uint[mesh->num_indices]; // assume each face is a triangle
				for (uint j = 0; j < ai_mesh->mNumFaces; ++j)
				{
					if (ai_mesh->mFaces[j].mNumIndices != 3)
					{
						CONSOLE_LOG("WARNING, geometry face with != 3 indices!");
						ret = false;
					}
					else
					{
						memcpy(&mesh->indices[j * 3], ai_mesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}
			}

			if (ai_mesh->HasNormals())
			{
				mesh->normals = new float[mesh->num_vertices * 3];
				memcpy(mesh->normals, ai_mesh->mNormals, sizeof(float) * mesh->num_vertices * 3);
			}

			if (ai_mesh->HasVertexColors(0))
			{
				mesh->colors = new float[mesh->num_vertices * 4];
				memcpy(mesh->colors, ai_mesh->mColors[0], sizeof(float) * mesh->num_vertices * 4);
			}

			if (ai_mesh->HasTextureCoords(0))
			{
				mesh->texture_coords = new float[mesh->num_vertices * 3];
				memcpy(mesh->texture_coords, ai_mesh->mTextureCoords[0], sizeof(float) * mesh->num_vertices * 3);
			}

			glGenBuffers(1, &mesh->id_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->num_vertices * 3, mesh->vertices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &mesh->id_indices);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_indices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*mesh->num_indices, mesh->indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			if (mesh->normals != nullptr)
			{
				glGenBuffers(1, &(mesh->id_normals));
				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * 3, mesh->normals, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			if (mesh->colors != nullptr)
			{
				glGenBuffers(1, &(mesh->id_colors));
				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_colors);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * 4, mesh->colors, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			if (mesh->texture_coords != nullptr)
			{
				glGenBuffers(1, &(mesh->id_texture_coords));
				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_texture_coords);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertices * 3, mesh->texture_coords, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			Texture* material_texture = nullptr;
			if (scene->HasMaterials())
			{
				aiMaterial* mat = scene->mMaterials[ai_mesh->mMaterialIndex];

				if (mat != nullptr)
				{
					aiString mat_texture_path;
					mat->GetTexture(aiTextureType_DIFFUSE, 0, &mat_texture_path);

					if (mat_texture_path.length > 0)
					{
						std::string full_texture_path;
						full_texture_path = TEXTURES_FOLDER + GetFileName(mat_texture_path.C_Str()) + GetFileExtension(mat_texture_path.C_Str());
						material_texture = LoadTexture(full_texture_path.c_str(), false);
					}
				}
			}

			GameObject* go = new GameObject(parent);
			ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)go->AddComponent(Component::MeshRenderer);
			mesh_renderer->LoadMesh(mesh);
			mesh_renderer->LoadTexture(material_texture);
			go->SetName((std::string)node->mName.C_Str());
			App->scene->AddGameObjectToScene(go);
		}
		root = parent;
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		LoadMeshNode(root, node->mChildren[i], scene, path);
	}
	
	return ret;
}

Texture* ModuleImport::LoadTexture(const char * path, bool attach_to_gameobject)
{
	//If gameobjects are not selected, don't load a texture. This is just for the assigment 1.
	//In the future if we drag a image without selected gameobjects, a texture should be created and stored in assets/library
	if (App->scene->selected_gameobjects.empty() && attach_to_gameobject) return nullptr;

	ILuint image_id;
	ilGenImages(1, &image_id);
	ilBindImage(image_id);

	Texture* tmp_texture = nullptr;

	if (ilLoadImage(path))
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		tmp_texture = new Texture();
		tmp_texture->SetID(ilutGLBindTexImage());
		tmp_texture->SetWidth(ImageInfo.Width);
		tmp_texture->SetHeight(ImageInfo.Height);
		tmp_texture->SetPath(path);
		tmp_texture->SetName(GetFileName(path).c_str());

		switch (ImageInfo.Format)
		{
			case IL_COLOUR_INDEX: tmp_texture->SetFormat(Texture::ColorIndex); break;
			case IL_ALPHA: tmp_texture->SetFormat(Texture::alpha); break;
			case IL_RGB: tmp_texture->SetFormat(Texture::rgb); break;
			case IL_RGBA: tmp_texture->SetFormat(Texture::rgba); break;
			case IL_BGR: tmp_texture->SetFormat(Texture::bgr); break;
			case IL_BGRA: tmp_texture->SetFormat(Texture::bgra); break;
			case IL_LUMINANCE: tmp_texture->SetFormat(Texture::luminance); break;
			case IL_LUMINANCE_ALPHA: tmp_texture->SetFormat(Texture::luminance_alpha); break;
			default: tmp_texture->SetFormat(Texture::UnknownFormat); break;
		}
		switch (ImageInfo.Type)
		{
			case IL_BMP: tmp_texture->SetType(Texture::TextureType::bmp); break;
			case IL_JPG: tmp_texture->SetType(Texture::TextureType::jpg); break;
			case IL_PNG: tmp_texture->SetType(Texture::TextureType::png); break;
			case IL_TGA: tmp_texture->SetType(Texture::TextureType::tga); break;
			case IL_DDS: tmp_texture->SetType(Texture::TextureType::dds); break;
			default: tmp_texture->SetType(Texture::TextureType::UnknownType); break;
		}
		ilDeleteImages(1, &image_id);
	}
	else 
	{
		CONSOLE_LOG("Cannot load image %s. Error: %s", path, iluErrorString(ilGetError()));
	}

	if (attach_to_gameobject) App->scene->ApplyTextureToSelectedGameObjects(tmp_texture);

	return tmp_texture;
}

std::string ModuleImport::GetFileName(const char * path)
{
	std::string name;
	std::string temp_path;
	temp_path = path;
	uint pos_bar = (temp_path.find(ASSETS_FOLDER) != std::string::npos) ? temp_path.find_last_of('/') : temp_path.find_last_of('\\');
	uint pos_point = temp_path.find_last_of('.');
	for (int i = pos_bar + 1; i < pos_point; i++)
	{
		name.push_back(temp_path[i]);
	}
	return name;
}

std::string ModuleImport::GetFileExtension(const char * path)
{
	std::string name;
	std::string temp_path;
	temp_path = path;
	uint pos_point = temp_path.find_last_of('.');
	for (int i = pos_point; i < temp_path.length(); i++)
	{
		name.push_back(temp_path[i]);
	}
	return name;
}

void ModuleImport::bla(char* message)
{
	int i = 0;
}

