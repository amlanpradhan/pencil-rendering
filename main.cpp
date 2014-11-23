#include <iostream>
#include <cstdlib>
#include "bmp.h"
#include <GLUT/glut.h>
#include "Texture.h"
#include "Shader.h"
#include "trimesh2/Trimesh.h"

GLubyte image[512][512][4];

#define WIND_WIDTH 512
#define WIND_HEIGHT 512

using namespace std;
using namespace trimesh;

static int g_window;

Shader shaderObj;
Texture textureObj;
TriMesh * mesh;

void readFromBMP()
{
    //RedleavesTexture.bmp, texture,bmp, texture30.bmp
    BMPLoader * bmpLoader = new BMPLoader();
    bmpLoader->loadBMP("RedleavesTexture.bmp");
   // std::cout<<bmpLoader->imageWidth<<" "<<bmpLoader->imageHeight<<"\n";
    textureObj.init(GL_RGB8, bmpLoader->imageWidth, bmpLoader->imageHeight, bmpLoader->image);
        //fclose(bmpInput);
}

void readFromMesh(char * filename)
{
    mesh = TriMesh::read(filename);
    if(!mesh)
        exit(1);

   /* int nv = m->vertices.size();

    int nf = m->faces.size();
    for(TriMesh::Face face : m->faces)
    {
        std::cout<<face.v[0]<<","<<face.v[1]<<","<<face.v[2]<<"\n";

    }*/
    //std::cout<<nf;
    //std::cout<<nv;
}

void drawScene()
{
    readFromMesh("cube.obj");
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, textureObj.getID());
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();
    
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
   // glOrtho(-512, 512, -512, 512, -200, 200);
   
    //glColor3f(1,0,0);
   // std::cout<<"Number of faces - "<<mesh->faces.size()<<"\n";
   /* for(TriMesh::Face face : mesh->faces)
    {
        int index1 = face.v[0]; int index2 = face.v[1]; int index3 = face.v[2];
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(mesh->vertices[index1][0], mesh->vertices[index1][1], mesh->vertices[index1][2]);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(mesh->vertices[index2][0], mesh->vertices[index2][1], mesh->vertices[index2][2]);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(mesh->vertices[index3][0], mesh->vertices[index3][1], mesh->vertices[index3][2]);
        glEnd();
       // std::cout<<point[0]<<","<<point[1]<<","<<point[2]<<"\n";
    } */
  /*  glBegin(GL_TRIANGLES);
    glVertex3f(0,0,0);
    glVertex3f(0,300,0);
    glVertex3f(300,300,0);
    //glVertex3f(300,0,0);
    glEnd();*/
}

void renderScene()
{
   // glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, -1, 0, 0, 0, 0, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);

	//glBegin(g_ViewMode);

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, textureBuffer.getID());
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawScene();
    glutSwapBuffers();
}

static void handleKeyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        default:
        	break;
        case 27:
        	glutDestroyWindow(g_window);
        	exit(0);
        	break;
    }
}

void display()
{
    
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIND_WIDTH, WIND_HEIGHT);
	g_window = glutCreateWindow("Shader program");
	glutDisplayFunc(renderScene);
	//glutIdleFunc(renderScene);
	//glutReshapeFunc(changeSize);
	glutKeyboardFunc(handleKeyboard);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

   // shaderObj = new Shader();
    
	shaderObj.setShader("textureBuf", "textureBuf");
	readFromBMP();
	glutMainLoop();
	return 0;
}
