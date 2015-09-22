#ifndef __CCRIFT_COMMONS_H__
#define __CCRIFT_COMMONS_H__

#ifndef VALIDATE
#define VALIDATE(x, msg) if (!(x)) { MessageBoxA(NULL, (msg), "OculusRoomTiny", MB_ICONERROR | MB_OK); exit(-1); }
#endif

#define M_PI 3.14159265f


#include "CCRiftBufferHelpers.h"

//static BasicTexture* loadBMP_custom(const char * imagepath)
//{
//	printf("Reading image %s\n", imagepath);
//
//	// Data read from the header of the BMP file
//	unsigned char header[54];
//	unsigned int dataPos;
//	unsigned int imageSize;
//	unsigned int width, height;
//	// Actual RGB data
//	unsigned char * data;
//
//	// Open the file
//	FILE * file = fopen(imagepath, "rb");
//	if (!file)							    { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }
//
//	// Read the header, i.e. the 54 first bytes
//
//	// If less than 54 bytes are read, problem
//	if (fread(header, 1, 54, file) != 54){
//		printf("Not a correct BMP file\n");
//		return 0;
//	}
//	// A BMP files always begins with "BM"
//	if (header[0] != 'B' || header[1] != 'M'){
//		printf("Not a correct BMP file\n");
//		return 0;
//	}
//	// Make sure this is a 24bpp file
//	if (*(int*)&(header[0x1E]) != 0)         { printf("Not a correct BMP file\n");    return 0; }
//	if (*(int*)&(header[0x1C]) != 24)         { printf("Not a correct BMP file\n");    return 0; }
//
//	// Read the information about the image
//	dataPos = *(int*)&(header[0x0A]);
//	imageSize = *(int*)&(header[0x22]);
//	width = *(int*)&(header[0x12]);
//	height = *(int*)&(header[0x16]);
//
//	// Some BMP files are misformatted, guess missing information
//	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
//	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
//
//	// Create a buffer
//	data = new unsigned char[imageSize];
//
//	// Read the actual data from the file into the buffer
//	fread(data, 1, imageSize, file);
//
//	// Everything is in memory now, the file wan be closed
//	fclose(file);
//
//	BasicTexture* tb = new BasicTexture(Sizei(width, height), false, false, 1, data);
//
//	delete[] data;
//
//	return tb;
//}

#endif //__CCRIFT_COMMONS_H__s