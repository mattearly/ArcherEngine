#pragma once
#include <AL/al.h>
namespace AA {
static void local_alErrorCheck() {
  auto error = alGetError();
  switch (error) {
  case AL_INVALID_NAME:
    throw("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
  case AL_INVALID_ENUM:
    throw("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
  case AL_INVALID_VALUE:
    throw("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
  case AL_INVALID_OPERATION:
    throw("AL_INVALID_OPERATION: the requested operation is not valid");
  case AL_OUT_OF_MEMORY:
    throw("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
  default:
    break;
  }
}
} // end namespace AA