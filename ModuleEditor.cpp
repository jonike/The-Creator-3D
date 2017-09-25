#include "ModuleEditor.h"
#include "Application.h"
//#include "tinyfiledialogs.h"
#include "imgui_impl_sdl.h"
#include "HierarchyWindow.h"
#include "PropertiesWindow.h"
#include "AssetsWindow.h"
#include "ConsoleWindow.h"
#include "SceneWindow.h"
#include "ModuleWindow.h"
#include "HardwareWindow.h"
#include "PerformanceWindow.h"
#include "AboutWindow.h"
#include "AppWindowConfigWindow.h"

ModuleEditor::ModuleEditor(Application * app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	ImGui_ImplSdl_Init(App->window->window);

	// (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../Source/imgui-1.51/extra_fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../Source/imgui-1.51/extra_fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../Source/imgui-1.51/extra_fonts/ProggyClean.ttf", 13.0f);
	//io.Fonts->AddFontFromFileTTF("../Source/imgui-1.51/extra_fonts/ProggyTiny.ttf", 10.0f);
	//font = io.Fonts->AddFontFromFileTTF("../Data/OpenSans-Semibold.ttf", 16);
	
	editor_windows.push_back(scene_window = new SceneWindow());
	editor_windows.push_back(assets_window = new AssetsWindow());
	editor_windows.push_back(hierarchy_window = new HierarchyWindow());
	editor_windows.push_back(properties_window = new PropertiesWindow());
	editor_windows.push_back(console_window = new ConsoleWindow());
	editor_windows.push_back(hardware_window = new HardwareWindow());
	editor_windows.push_back(performance_window = new PerformanceWindow());
	editor_windows.push_back(about_window = new AboutWindow());
	editor_windows.push_back(config_window = new AppWindowConfigWindow());
	//editor_panels.push_back(animator_panel = new PanelAnimator());
	//editor_panels.push_back(particle_editor_panel = new PanelParticleEditor());
	ImGui::LoadDocks();
	return true;
}

update_status ModuleEditor::PreUpdate(float delta_time)
{
	ImGui_ImplSdl_NewFrame(App->window->window);
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float deltaTime)
{
	bool ret = true;
	ImGui::PushFont(font);
	if (ImGui::BeginMainMenuBar())
	{
		bool selected = false;
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4 previous_color = style.Colors[ImGuiCol_Text];
		style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		if (ImGui::BeginMenu("File"))
		{
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			//if (ImGui::MenuItem("New Scene")) {
			//	//App->scene_manager_module->NewScene();
			//}
			//if (ImGui::MenuItem("Load Scene")) {
			//	char const * lFilterPatterns[1] = { "*.scene" };
			//	//const char* path = tinyfd_openFileDialog("Load Scene...", NULL, 1, lFilterPatterns, NULL, 0);
			//	//if (path != NULL) {
			//	//	//App->scene_manager_module->LoadScene(path);
			//	//}
			//}
			//if (ImGui::MenuItem("Save Scene")) {
			//	/*char const * lFilterPatterns[1] = { "*.scene" };
			//	const char* path = tinyfd_saveFileDialog("Save Scene...", (App->scene_manager_module->scene_name + ".scene").c_str(), 1, lFilterPatterns, NULL);
			//	if (path != NULL) {
			//		std::string str(path);
			//		bool get_char = false;
			//		std::string new_scene_name;
			//		for (std::string::reverse_iterator it = str.rbegin(); it != str.rend(); it++) {
			//			if (*it == '\\') {
			//				get_char = false;
			//			}
			//			if (get_char) {
			//				new_scene_name.insert(0, 1, *it);
			//			}
			//			if (*it == '.') {
			//				get_char = true;
			//			}
			//		}
			//		App->engine_Window->SetSceneName(new_scene_name);
			//		App->scene_manager_module->SaveScene(path);
			//		App->scene_manager_module->saving_index = 0;
			//	}*/
			//}
			if (ImGui::MenuItem("Exit")) {
				return UPDATE_STOP;
			}

			ImGui::EndMenu();
			style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		}

		if (ImGui::BeginMenu("Windows"))
		{
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			if (ImGui::MenuItem("Hardware"))
			{
				hardware_window->active = !hardware_window->active;
			}
			if (ImGui::MenuItem("Performance"))
			{
				performance_window->active = !performance_window->active;
			}
			if (ImGui::MenuItem("WindowConfig"))
			{
				config_window->active = !config_window->active;
			}
			ImGui::EndMenu();
			style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		}

		if (ImGui::BeginMenu("Help"))
		{
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			if (ImGui::MenuItem("About..."))
			{
				about_window->active = !about_window->active;
			}
			if (ImGui::MenuItem("Documentation"))
			{
				OpenBrowserPage("https://github.com/sergipa/The-Creator-3D/wiki");
			}
			if (ImGui::MenuItem("Download Latest"))
			{
				OpenBrowserPage("https://github.com/sergipa/The-Creator-3D/releases");
			}
			if (ImGui::MenuItem("Report a bug"))
			{
				OpenBrowserPage("https://github.com/sergipa/The-Creator-3D/issues");
			}
			ImGui::EndMenu();
			style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		}
		ImGui::EndMainMenuBar();
		style.Colors[ImGuiCol_Text] = previous_color;
	}

	ImVec2 display_size = ImGui::GetIO().DisplaySize;
	display_size.y -= 10;
	ImGui::SetNextWindowSize(display_size);
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::Begin("PanelEditor", NULL, ImVec2(0, 0), 1.0f, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);

	ImGui::SameLine(ImGui::GetIO().DisplaySize.x / 2 - 75);
	if (ImGui::Button("Play", { 50,40 })) {
		//App->Play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop", { 50,40 })) {
		//App->Stop();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause", { 50,40 })) {

	}

	//ImGui::SameLine();
	//ImGui::Text("%f", App->GetFPS());

	ImGui::Separator();
	ImGui::BeginDockspace();
	for (std::vector<Window*>::iterator it = editor_windows.begin(); it != editor_windows.end(); it++) {
		if ((*it)->IsActive())
		{
			(*it)->DrawWindow();
		}
	}
	ImGui::EndDockspace();
	ImGui::End();
	ImGui::PopFont();
	return UPDATE_CONTINUE;
}

bool ModuleEditor::DrawEditor()
{
	ImGui::Render();
	return true;
}

bool ModuleEditor::CleanUp()
{
	for (std::vector<Window*>::iterator it = editor_windows.begin(); it != editor_windows.end(); ++it) {
		RELEASE(*it);
	}
	editor_windows.clear();
	ImGui::SaveDocks();
	ImGui_ImplSdl_Shutdown();
	return true;
}

void ModuleEditor::HandleInput(SDL_Event * event)
{
	ImGui_ImplSdl_ProcessEvent(event);
}

void ModuleEditor::OpenBrowserPage(const char * url)
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void ModuleEditor::AddData_Editor(float ms, float fps)
{
	performance_window->AddData(ms, fps);
}
