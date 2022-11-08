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
static AA::Interface* file_interface_ref = nullptr;
static int file_menu_imgui_id = -1;
static bool show_controls = false;
static bool show_model_panel = false;

// setup function
void setup_file_menu(AA::Interface& interface) {
  if (file_interface_ref) return;  // already setup, don't do it twice
  file_interface_ref = &interface;
  file_menu_imgui_id = file_interface_ref->AddToImGuiUpdate([]() {
    // THE TOP MAIN MENU BAR
    if (ImGui::BeginMainMenuBar())
    {
      // MAIN MENU BAR OPTION FILE
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Import Model File...", "", nullptr)) {
          /* todo */
          std::string file{};
          bool ok = AA::NavigateFileSystem(file, "fbx,glb,obj,dae", "");
          if (ok)
            file_interface_ref->AddProp(file.c_str(), false);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "alt+f4", nullptr)) { file_interface_ref->Shutdown(); }
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
      std::vector<uint32_t> prop_ids = file_interface_ref->GetAllPropIds();
      for (const auto& id : prop_ids) {
        std::stringstream ss;
        ss << id;
        std::string the_text = "MODEL ID: " + ss.str();
        ImGui::Text(the_text.c_str());
      }
      ImGui::End();
    }
  });
}

// teardown function
void remove_file_menu() {
  if (!file_interface_ref) return;  // not setup, don't run
  file_interface_ref->RemoveFromImGuiUpdate(file_menu_imgui_id);
  file_interface_ref = nullptr;
  file_menu_imgui_id = -1;
  show_model_panel = false;
  show_controls = false;
}
