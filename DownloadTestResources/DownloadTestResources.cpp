/*  Written to download the files necessary to run all the tests.
    Run this before running all the tests to have it download yoru assest.
    You then need to unzip the assests into the 'RuntimeFiles' directory.
- MJE
*/

#include <iostream>
#include <curl/curl.h>
#include <direct.h>

using namespace std;

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  return fwrite(ptr, size, nmemb, stream);
}

int main() {
  CURL* curl;
  curl = curl_easy_init();

  const char* destination = "../RuntimeFiles.zip";
  FILE* filepath;
  // check if file exists already
  auto test_if_exists_err_code = fopen_s(&filepath, destination, "r");
  if (test_if_exists_err_code == 0) {
    std::cout << "file already exits, unzip it to RuntimeFiles dir\n";
    return EXIT_SUCCESS;
  }
  else {
    std::cout << "creating output file...\n";
    auto fopen_err_code = fopen_s(&filepath, destination, "wb");
    if (fopen_err_code != 0) {
      std::cout << "error creating output file\n";
      return EXIT_FAILURE;
    }
  }




  /* A long parameter set to 1 tells the library to follow any Location: header
 * that the server sends as part of an HTTP header in a 3xx response. The
 *Location: header can specify a relative or an absolute URL to follow.
*/
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_URL, "https://www.dropbox.com/s/cqn4p7cp07odxdt/RuntimeFiles.zip?dl=1"); // "dl=0"changed to "dl=1" to force download

  // disabe the SSL peer certificate verification allowing the program to download the file from dropbox shared link
// in case it is not used it displays an error message stating "SSL peer certificate or SSH remote key was not OK"
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

  curl_easy_setopt(curl, CURLOPT_WRITEDATA, filepath);

  CURLcode res;
  std::cout << "performing download...\n";
  res = curl_easy_perform(curl);
  std::cout << "cleaning up curl...\n";
  curl_easy_cleanup(curl);
  fclose(filepath);

  if (res == CURLE_OK) {
    // good
  } else {
    cout << curl_easy_strerror(res);
    return EXIT_FAILURE;
  }

  // make the required directory
  auto mkdir_return_code = _mkdir("..\\RuntimeFiles");
  if (mkdir_return_code != 0) {
    switch (errno) {
    case EEXIST: 
      cout << "directory already exists, unzip to RuntimeFiles/ folder\n";
      //system("unzip ../RuntimeFiles.zip -d ../RuntimeFiles/");
      break;
    case ENOENT:
      cout << "mkdir path was not found\n";
      break;
    default:
      cout << "unspecified mkdir error\n";
      break;
    }
  } else {
    cout << "directory created, unzip to RuntimeFiles/ folder\n";
    //system("unzip ../RuntimeFiles.zip -d ../RuntimeFiles/");
  }

  return EXIT_SUCCESS;
}