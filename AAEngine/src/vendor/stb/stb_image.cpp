#define STB_IMAGE_IMPLEMENTATION

// png, jpegs, and bmp only

//#define STBI_NO_JPEG
//#define STBI_NO_PNG
//#define STBI_NO_BMP
#define STBI_NO_PSD
//#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM  (.ppm and .pgm)

#define STBI_WINDOWS_UTF8

#define STBI_NO_FAILURE_STRINGS

//#define STBI_ONLY_PNG

//#define STBI_ASSERT(x)

//#define STBI_MALLOC malloc
//#define STBI_REALLOC realloc
//#define STBI_FREE free

#include <stb_image.h>