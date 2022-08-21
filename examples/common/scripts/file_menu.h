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

// globals local to this file
AA::Interface* interface_ref = nullptr;
int file_menu_imgui_id = -1;
bool menu_is_setup = false;
bool show_controls = false;

// setup function
void setup_file_menu(AA::Interface& interface) {
  if (menu_is_setup) return;  // already setup, don't do it twice
  interface_ref = &interface;

  file_menu_imgui_id = interface_ref->AddToImGuiUpdate([]() {
    // THE MENUS
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Import Model File...", "", nullptr)) {
          /* todo */
          std::string file{};
          bool ok = AA::NavigateFileSystem(file, "fbx,glb,obj,dae", "");
          if (ok)
            interface_ref->AddProp(file.c_str(), false);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "alt+f4", nullptr)) { interface_ref->Shutdown(); }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Help")) {
        ImGui::MenuItem("Show Controls", "", &show_controls);
        ImGui::EndMenu();
      }
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
  });

  menu_is_setup = true;
}

// teardown function
void remove_file_menu() {
  if (!menu_is_setup) return;  // not setup, don't run
  interface_ref->RemoveFromImGuiUpdate(file_menu_imgui_id);
  interface_ref = nullptr;
  file_menu_imgui_id = -1;
  menu_is_setup = false;
}
