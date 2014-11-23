#include "bmp.h"
#include <cstdlib>
#include <iostream>

BMPLoader::BMPLoader()
{
	image = 0;
	imageWidth = 0;
	imageHeight = 0;
}

BMPLoader::~BMPLoader()
{
	//FreeImage();
}

void testLines()
{
    
}

/**void LoadTexture() {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath("hazard.png"));
    bmp.flipVertically();
    gTexture = new tdogl::Texture(bmp);
}**/

bool BMPLoader::loadBMP(const char * file)
{
	GLint totalBytes, lineBytes;

	FILE *pFile = fopen(file, "rb");
	if(!pFile)
	{
		//Include a debug statement here
		return false;
	}

	fseek(pFile, 0x0012, SEEK_SET);
	fread(&imageWidth, 4, 1, pFile);
	fread(&imageHeight, 4, 1, pFile);
	fseek(pFile, 54, SEEK_SET);

	lineBytes = imageWidth*3;
	while(lineBytes%4 != 0)
		lineBytes++;

	totalBytes = lineBytes * imageHeight;

	image = new GLubyte[totalBytes];
	fread(image, totalBytes, 1, pFile);
	/*if (fread(image, totalBytes, 1, pFile) <= 0)
	{
		delete image;
		fclose(pFile);
		return 0;
	}*/
    
	//std::cout<<val<<" "<<file[1];
	//for(int i=0; i<totalBytes; i++)
	//	std::cout<<image[i]<<" ";

    
	//Do error checking here

	char textureColors;
	for(int index=0; index<totalBytes; index+=3)
	{
		textureColors = image[index];
		image[index] = image[index+2];
		image[index+2] = textureColors;
	}

	fclose(pFile);
	return true;
}