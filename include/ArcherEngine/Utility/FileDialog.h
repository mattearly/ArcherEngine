#pragma once
#include <string>
namespace AA {

/// <summary>
/// Opens your Operating System's Native File Dialog and allows you to navigate to a file, clicking Okay or Cancel.
/// </summary>
/// <param name="out"> a filled out string of the path you navigated to. </param>
/// <param name="filetypereg">file type endings (after .), comma seperated. </param>
/// <param name="starting_pref">a path to attempt to start at. if failed, should start at last accessed path on windows. </param>
/// <returns>true if okay was pressed, false if cancel was pressed</returns>
bool NavigateFileSystem(std::string& out, const char* filetypereg, const char* starting_pref);

}