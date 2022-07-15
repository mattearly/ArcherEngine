#include "AssimpSceneLoader.h"
namespace AA {
bool AssimpSceneLoader::doLogging = false;

static C_STRUCT aiLogStream logstream;

void AssimpSceneLoader::Logging(const bool& on_off, const bool& to_file) noexcept {
  if (on_off) {
    if (to_file) {
      /*this stream now writes the log messages e assimp_log.txt */
      logstream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "ae_model_loader_logs.txt");
      aiAttachLogStream(&logstream);
    } else {
      /*this stream now writes the log messages e assimp_log.txt */
      logstream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
      aiAttachLogStream(&logstream);
    }
  } else {
    // turn off
    aiDetachAllLogStreams();
  }
}

}