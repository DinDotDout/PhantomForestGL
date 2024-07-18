#ifndef __BMP_LOADER_H
#define __BMP_LOADER_H

unsigned char * loadBMPRaw(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight, bool flipY = true);

#endif