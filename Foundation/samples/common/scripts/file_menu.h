#pragma once
#include <ArcherEngine/Interface.h>
#include <ArcherEngine/Utility/FileDialog.h>
#ifdef __linux__
#include <imgui/imgui.h>
#elif _WIN32
#include <imgui.h>
#endif
#include <memory>
#include <string>
#include <sstream>

// globals local to this file
static AA::Interface* aa_engine_ref = nullptr;
static int file_menu_imgui_id = -1;
static bool show_controls = false;
static bool show_model_panel = false;
static unsigned int cam_id_ref = 0;
// setup function
void setup_file_menu(const unsigned int& cam_id, AA::Interface& interface) {
  if (aa_engine_ref) return;  // already setup, don't do it twice
  aa_engine_ref = &interface;
  cam_id_ref = cam_id;
  file_menu_imgui_id = aa_engine_ref->AddToImGuiUpdate([]() {
    // THE TOP MAIN MENU BAR
    if (ImGui::BeginMainMenuBar())
    {
      // MAIN MENU BAR OPTION FILE
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Import Model File...", "", nullptr)) {
          std::string file{};
          bool ok = AA::NavigateFileSystem(file, "fbx,glb,obj,dae", "");
          if (ok)
            aa_engine_ref->AddProp(file.c_str(), false);
        }
        if (ImGui::MenuItem("Import Skybox Files (order:udlfrb)", "", nullptr)) {
          // todo: this does a no-existext job oof telling the user which to load.
          //   they should be picked in order of up down left front right back
          std::string up{}, down{}, left{}, front{}, right{}, back{};
          const char* types = "png,tga,jpg,jpeg,gif,tiff,bmp";
          bool has_up = AA::NavigateFileSystem(up, types, "");
          bool has_down = AA::NavigateFileSystem(down, types, "");
          bool has_left = AA::NavigateFileSystem(left, types, "");
          bool has_front = AA::NavigateFileSystem(front, types, "");
          bool has_right = AA::NavigateFileSystem(right, types, "");
          bool has_back = AA::NavigateFileSystem(back, types, "");
          if (has_up &&
            has_down &&
            has_left &&
            has_front &&
            has_right &&
            has_back) {
            //however internally are loaded in order of right, left, up, down, front, back
            std::vector<std::string> skybox_files{ right, left, up, down, front, back };
            aa_engine_ref->GetCamera(cam_id_ref).lock()->SetSkybox(skybox_files);
          }
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "alt+f4", nullptr)) { aa_engine_ref->Shutdown(); }
        ImGui::EndMenu();
      }
      // MAIN MENU BAR OPTION PANEL

      if (ImGui::BeginMenu("Panels")) {
        ImGui::MenuItem("Show Model Details Pane", "", &show_model_panel);
        ImGui::EndMenu();
      }

      // MAIN MENU BAR OPTION HELP
      if (ImGui::BeginMenu("Help")) {
        ImGui::MenuItem("Show Controls", "", &show_controls);
        ImGui::EndMenu();
      }
      // END MAIN MENU BAR
      ImGui::EndMainMenuBar();
    }

  // CONDITIONALS
  if (show_controls) {
    ImGui::Begin("Controls");
    ImGui::Text("TOGGLE MOUSE: right click");
    ImGui::Text("MOVE: wasd");
    ImGui::Text("LOOK: mouse (when toggled)");
    ImGui::Text("FLY UP: spacebar");
    ImGui::Text("FLY DOWN: c");
    ImGui::End();
  }
  if (show_model_panel) {
    ImGui::Begin("Models");
    // test to see if we can get a list of models showing
    std::vector<uint32_t> prop_ids = aa_engine_ref->GetAllPropIds();
    for (const auto& id : prop_ids) {
      std::stringstream ss;

      // model id (unique identifier)
      ss << id;
      std::string id_text = "ID: " + ss.str();
      ImGui::Text(id_text.c_str());

      // get other stats
      auto model_ref = aa_engine_ref->GetProp(id).lock();

      // model location
      ss.str(std::string());
      auto loc = model_ref->GetLocation();
      ss << "Location: " << loc.x << ", " << loc.y << ", " << loc.z;
      std::string loc_string = ss.str();
      ImGui::Text(loc_string.c_str());

    }
    ImGui::End();
  }
  });
}

// teardown function
void remove_file_menu() {
  if (!aa_engine_ref) return;  // not setup, don't run
  aa_engine_ref->RemoveFromImGuiUpdate(file_menu_imgui_id);
  aa_engine_ref = nullptr;
  file_menu_imgui_id = -1;
  show_model_panel = false;
  show_controls = false;
}
