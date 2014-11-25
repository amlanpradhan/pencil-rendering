#include <iostream>
#include <cstdlib>
#include "bmp.h"
#include <GLUT/glut.h>
#include "Texture.h"
#include "Shader.h"
#include "trimesh2/Trimesh.h"

GLubyte image[32][512][512][4];

#define WIND_WIDTH 512
#define WIND_HEIGHT 512

using namespace std;
using namespace trimesh;

static int g_window;

Shader nprShader, curvShader, zBufShader, shaderObj;
Texture zBufTexture, pencilTexture, curvTexture;
Texture textureObj;
TriMesh * mesh;

void initialize()
{
    float ambLight[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    float diffLight[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float specLight[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specLight);


    float ambMat[4] = {0.3f, 0.3f, 0.3f, 1.0f};
    float diffMat[4] = {0.9f, 0.5f, 0.5f, 1.0f};
    float specMat[4] = {0.6f, 0.6f, 0.6f, 1.0f};

    glLightfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambMat);
    glLightfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffMat);
    glLightfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat);

    //TODO: add shininess factor

    GLfloat light_pos[4] = {0.0f, 0.0f, -1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glShadeModel(GL_LIGHT0);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    std::cout<<"here\n";

    zBufShader.setShader("zBuf", "zBuf");
    nprShader.setShader("npr", "npr");
    curvShader.setShader("curv", "curv");

    std::cout<<"here2\n";

    zBufTexture.init(GL_RGBA8, WIND_WIDTH, WIND_HEIGHT);
    curvTexture.init(GL_RGBA8, WIND_WIDTH, WIND_HEIGHT);

    BMPLoader *bmpLoader = new BMPLoader();

    std::cout<<"here3\n";

    for(int i=0; i<32; i++)
    {
        char filename[100];
        sprintf(filename, "texture/texture%02d.bmp", i+1);
        FILE *bmpInput = fopen(filename, "rb");
        fseek(bmpInput, 0x436L, SEEK_SET);
        for(int j=0; j<512; j++)
        {
            for(int k=0; k<512; k++)
            {
                fread(&image[i][j][k][0], 1, 1, bmpInput);
                image[i][j][k][1] = image[i][j][k][0];
                image[i][j][k][2] = image[i][j][k][0];
                image[i][j][k][3] = 0;
            }
        }
    }

    pencilTexture.init(GL_RGB8, WIND_WIDTH, WIND_WIDTH, 32, &image[0][0][0][0]);
}

/*void readFromBMP()
{
    //RedleavesTexture.bmp, texture,bmp, texture30.bmp
    BMPLoader * bmpLoader = new BMPLoader();
    bmpLoader->loadBMP("texture30.bmp");
   // std::cout<<bmpLoader->imageWidth<<" "<<bmpLoader->imageHeight<<"\n";
    textureObj.init(GL_RGB8, bmpLoader->imageWidth, bmpLoader->imageHeight, bmpLoader->image);
        //fclose(bmpInput);
}*/

void readFromMesh(char * filename)
{
    mesh = TriMesh::read(filename);
    if(!mesh)
        exit(1);

    int nv = mesh->vertices.size();

    int nf = mesh->faces.size();
    mesh->need_curvatures();
   /* for(TriMesh::Face face : mesh->faces)
    {
        std::cout<<face.v[0]<<","<<face.v[1]<<","<<face.v[2]<<"\n";
    }*/

    int curvSize = mesh->curv1.size();
    for(int i=0; i<curvSize; i++)
    {
        //std::cout<<s
        std::cout<<mesh->curv1[i]<<":"<<mesh->curv2[i]<<":"<<mesh->normals[i]<<"\n";
    }
    //std::cout<<nf;
    //std::cout<<nv;
}

void drawScene()
{
    readFromMesh("cube.obj");
   /* glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, pencilTexture.getID());*/
   // glColor3f(1,0,0); 
	/*glBegin(GL_QUADS);
       // glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
       // glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
       // glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
       // glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
    glEnd();*/
    
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
   // glOrtho(-512, 512, -512, 512, -200, 200);
    glOrtho(-2, 2, -2, 2, -2, 2);
   // std::cout<<"Number of faces - "<<mesh->faces.size()<<"\n";
    glEnable(GL_TEXTURE_3D);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pencilTexture.getID());
    for(TriMesh::Face face : mesh->faces)
    {
        int index1 = face.v[0]; int index2 = face.v[1]; int index3 = face.v[2];
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(mesh->vertices[index1][0], mesh->vertices[index1][1], mesh->vertices[index1][2]);
        glTexCoord2f(0.0f, 0.5f); glVertex3f(mesh->vertices[index2][0], mesh->vertices[index2][1], mesh->vertices[index2][2]);
        glTexCoord2f(0.5f, 0.5f); glVertex3f(mesh->vertices[index3][0], mesh->vertices[index3][1], mesh->vertices[index3][2]);
        glEnd();
    } 
   /* glBegin(GL_QUADS);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,1,-1);
    glVertex3f(1,1,-1);
    glVertex3f(1,-1,-1);
    glEnd();*/
    glDisable(GL_TEXTURE_3D); 
}

void renderScene()
{
   // glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, -1, 0, 0, 0, 0, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//glEnable(GL_TEXTURE_2D);
	//glColor3f(1,1,1);
    initialize();
    //curvShader.enable();
	drawScene();
    //curvShader.disable();
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
    std::cout<<"Tick";
    
	g_window = glutCreateWindow("Shader program");
	glutDisplayFunc(renderScene);
	//glutIdleFunc(renderScene);
	//glutReshapeFunc(changeSize);
	glutKeyboardFunc(handleKeyboard);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //shaderObj = new Shader();
    //initialize();
    
	//shaderObj.setShader("zBuf", "zBuf");
	//readFromBMP();
	glutMainLoop();
	return 0;
}
