#include <Utility/FileDialog.h>
#include <nfd.h>
bool AA::NavigateFileSystem(std::string& out, const char* filetypereg, const char* starting_pref)
{
  bool func_result = true;
  char* model_file = NULL;
  nfdresult_t result = NFD_OpenDialog(filetypereg, starting_pref, &model_file);
  if (result == NFD_OKAY) {
    out = model_file;
  }
  else if (result == NFD_CANCEL) {
    func_result = false;
  }
  else {
    func_result = false;
  }

  return func_result;
}
