#pragma once
#include <ArcherEngine/Interface.h>
#include <imgui.h>
#include <memory>

// globals local to this file
AA::Interface* interface_ref = nullptr;
int file_menu_imgui_id = -1;
bool menu_is_setup = false;

// setup function
void setup_file_menu(AA::Interface& interface) {
  if (menu_is_setup) return;  // already setup, don't do it twice
  interface_ref = &interface;

  file_menu_imgui_id = interface_ref->AddToImGuiUpdate([](){
    bool quit = false;

    // Menu
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File")) {
        quit = ImGui::MenuItem("Exit", "alt+f4", nullptr);
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    if (quit) interface_ref->Shutdown();
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
