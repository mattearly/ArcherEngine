#include <Utility/FileDialog.h>



#ifdef __linux__
#include <stdio.h>
bool AA::NavigateFileSystem(std::string& out, const char* filetypereg, const char* starting_pref)
{
  printf("opening file...");
  out.clear();
  bool func_result = true;
  const int PATH_MAX = 1024;
  int status;
  char path[PATH_MAX];

  FILE *fp;
  fp = popen("zenity --file-selection", "r");
  if (fp == NULL) {
    func_result = false;
    /* Handle error */
    printf("path failure: %s", path);
  } else {
    fgets(path, PATH_MAX, fp);
    std::string string_ver = path;
    auto end_pos = string_ver.find_first_of('\n');
    out = string_ver.substr(0, end_pos);

  }
  status = pclose(fp);
  if (status == -1) {
    /* Error reported by pclose() */
     printf("error by pclose() %s", path);
  } else {
    /* Use macros described under wait() to inspect `status' in order
       to determine success/failure of command executed by popen() */
  }
  printf("out path is: %s", out);
  return func_result;
}



#elif _WIN32
#include <nfd.h>
bool AA::NavigateFileSystem(std::string& out, const char* filetypereg, const char* starting_pref)
{
  out.clear();
  bool func_result = true;
  char* file_hunted = NULL;
  nfdresult_t result = NFD_OpenDialog(filetypereg, starting_pref, &file_hunted);
  if (result == NFD_OKAY) {
    out = file_hunted;
  }
  else if (result == NFD_CANCEL) {
    func_result = false;
  }
  else {
    func_result = false;
  }
  return func_result;
}



#endif



