#ifndef __BMP_H__ 
#define __BMP_H__ 

#include <GLUT/glut.h>
#include <cstdio>

#define BITMAP_ID 0x4D42

class BMPLoader
{
	public:
		BMPLoader();
		~BMPLoader();

		bool loadBMP(const char *filename);
		//bool saveBMP(const char *, GLint, GLint, GLint, GLubyte *);

		int imageWidth;
		int imageHeight;
		unsigned char * image;
};

#endif

