#pragma once
#include <string>
#include <vector>
#include <memory>
#include <AncientArcher/AncientArcher.h>
extern AA::AncientArcher instance;
void setskybox() {
  const std::string skyboxfolder = "res/skybox/night/";
  //const std::string order[6] = { "posx", "negx", "posy", "negy", "posz", "negz" };
  //const std::string skyboxfileext = ".jpg";
  const std::string order[6] = { "right", "left", "top", "bottom", "front", "back" };
  const std::string skyboxfileext = ".png";
  std::vector<std::string> cubemapfiles;
  cubemapfiles.reserve(6);
  for (int j = 0; j < 6; ++j)
    cubemapfiles.emplace_back(skyboxfolder + order[j] + skyboxfileext);
  instance.SetSkybox(cubemapfiles);

}