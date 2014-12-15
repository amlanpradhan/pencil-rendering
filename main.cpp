#include <iostream>
#include <cstdlib>
#include "bmp.h"
#include "fbo.h"
#include <GLUT/glut.h>
#include "Texture.h"
#include "Shader.h"
#include "trimesh2/Trimesh.h"

GLubyte image[32][512][512][4];
GLubyte tempImage[512][512][4];

#define WIND_WIDTH 512
#define WIND_HEIGHT 512
#define GL_RGBA16F 0x881A

using namespace std;
using namespace trimesh;

static int g_window;

Shader nprShader, curvShader, zBufShader, testShader;
Texture zBufTexture, pencilTexture, curvTexture, tempTexture;
TriMesh * mesh;
FBO *curvatureFBO, *zBufFBO;

int xMin, yMin, xMax, yMax, zMin, zMax;

void initializeLightValues()
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

    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
}

void computeBoundaryPoints()
{
    xMin = mesh->vertices[0][0]; xMax = xMin;
    yMin = mesh->vertices[0][1]; yMax = yMin;
    zMin = mesh->vertices[0][2]; zMax = zMin;
    for(TriMesh::Face face : mesh->faces)
    {
        int index1 = face.v[0]; int index2 = face.v[1]; int index3 = face.v[2];
        int index;
        for(int i=0; i<3; i++)
        {
            index = face.v[i];
            int val = (int)mesh->vertices[index][0];
            if(xMin>val)xMin = val-3;
            else if(xMax<val)xMax = val+3;

            val = (int)mesh->vertices[index][1];
            if(yMin>val)yMin = val-3;
            else if(yMax<val)yMax = val+3;

            val = (int)mesh->vertices[index][2];
            if(zMin>val)zMin = val-3;
            else if(zMax<val)zMax = val+3;
        }
    }
}

void readFromMesh(char * filename)
{
    mesh = TriMesh::read(filename);
    if(!mesh)
        exit(1);

    int nv = mesh->vertices.size();

    int nf = mesh->faces.size();
    mesh->need_curvatures();

    int curvSize = mesh->curv1.size();
}

void initShaderObjects()
{
    zBufShader.setShader("zBuffer", "zBuffer");
    nprShader.setShader("npr", "npr");
    curvShader.setShader("curv", "curv");
    testShader.setShader("npr", "npr");

    zBufTexture.init(GL_RGBA16F, WIND_WIDTH, WIND_HEIGHT);
    curvTexture.init(GL_RGBA16F, WIND_WIDTH, WIND_HEIGHT);
    
    curvatureFBO = new FBO(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, curvTexture.getID());
    curvatureFBO->updateRBO(WIND_WIDTH, WIND_HEIGHT);
    zBufFBO = new FBO(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, zBufTexture.getID());
    zBufFBO->updateRBO(WIND_WIDTH, WIND_HEIGHT);
}

void readFromImage()
{
    BMPLoader *bmpLoader = new BMPLoader();

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

void readFromPicture()
{
    BMPLoader *bmpLoader = new BMPLoader();
    char filename[100];
    sprintf(filename, "texture30.bmp");
    FILE *bmpInput = fopen(filename, "rb");
    fseek(bmpInput, 0x436L, SEEK_SET);
    for(int i=0; i<512; i++)
    {
        for(int j=0; j<512; j++)
        {
            fread(&tempImage[i][j][0], 1, 1, bmpInput);
                tempImage[i][j][1] = tempImage[i][j][0];
                tempImage[i][j][2] = tempImage[i][j][0];
                tempImage[i][j][3] = 0;
        }
    }

    tempTexture.init(GL_RGB8, WIND_WIDTH, WIND_HEIGHT, &tempImage[0][0][0]);
}

void populateZBufferFBO()
{
    zBufShader.enable();
    zBufFBO->bindFBO();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    for(TriMesh::Face face : mesh->faces)
    {
        int index1 = face.v[0]; int index2 = face.v[1]; int index3 = face.v[2];
        int index;
        glBegin(GL_TRIANGLES);
        for(int i=0; i<3; i++)
        {
            index = face.v[i];
            glVertex3f(mesh->vertices[index][0], mesh->vertices[index][1], mesh->vertices[index][2]);
            zBufShader.setAttribute("curvatureDirection", mesh->pdir1[index][0], mesh->pdir1[index][1], mesh->pdir1[index][2]);
            zBufShader.setAttribute("minCurvatureDirection", mesh->pdir2[index][0], mesh->pdir2[index][1], mesh->pdir2[index][2]);
        }
        glEnd();
    }
    zBufFBO->unbindFBO();
    zBufShader.disable();
}

void populateCurvatureFBO()
{
    curvShader.enable();
    curvatureFBO->bindFBO();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_TEXTURE_2D);
    //glColor3ub(255, 255, 255);
    glColor3ub(0, 0, 0);
    for(TriMesh::Face face : mesh->faces)
    {
        int index1 = face.v[0]; int index2 = face.v[1]; int index3 = face.v[2];
        int index;
        glBegin(GL_TRIANGLES);
        for(int i=0; i<3; i++)
        {
            index = face.v[i];
            glVertex3f(mesh->vertices[index][0], mesh->vertices[index][1], mesh->vertices[index][2]);
            glNormal3f(mesh->normals[index][0], mesh->normals[index][1], mesh->normals[index][2]);
            curvShader.setAttribute("curvatureDirection", mesh->pdir1[index][0], mesh->pdir1[index][1], mesh->pdir1[index][2]);
            curvShader.setAttribute("minCurvatureDirection", mesh->pdir2[index][0], mesh->pdir2[index][1], mesh->pdir2[index][2]);
        }
        glEnd();
    }
    curvatureFBO->unbindFBO();
    curvShader.disable();
}

void initialize()
{
    initShaderObjects();
    initializeLightValues();
    readFromMesh("granade.obj");
    computeBoundaryPoints(); 
}

void testDraw()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tempTexture.getID());
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(xMin, yMin);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(xMax, yMin);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(xMax, yMax);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(xMin, yMax);
    glEnd();
}

void drawTexture()
{
    //glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, curvTexture.getID());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, zBufTexture.getID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pencilTexture.getID());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tempTexture.getID());
     //glBindTexture(GL_TEXTURE_2D, curvTexture.getID());
   
    //nprShader.enable();s
    testShader.enable();
    testShader.setUniform("texSrc", 0);
    testShader.setUniform("texPencil", 1);
    testShader.setUniform("texCur", 2);
    testShader.setUniform("tempTex", 3);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(xMin, yMin);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(xMax, yMin);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(xMax, yMax);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(xMin, yMax);
    glEnd(); 
    
   
    testShader.disable();

   // nprShader.disable(); 
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



void drawTest()
{
    glColor3f(1.0f, 0.0f, 0.0f);
    for(TriMesh::Face face : mesh->faces)
    {
        int index1 = face.v[0]; int index2 = face.v[1]; int index3 = face.v[2];
        glBegin(GL_TRIANGLES);
        glVertex3f(mesh->vertices[index1][0], mesh->vertices[index1][1], mesh->vertices[index1][2]);
        glVertex3f(mesh->vertices[index2][0], mesh->vertices[index2][1], mesh->vertices[index2][2]);
        glVertex3f(mesh->vertices[index3][0], mesh->vertices[index3][1], mesh->vertices[index3][2]);
        glEnd();
    } 
}

void drawScene()
{
    glOrtho(xMin, xMax, yMin, yMax, zMin, zMax);
    readFromImage();
    readFromPicture();
    populateZBufferFBO();
    populateCurvatureFBO();
    drawTexture();
}

void renderScene()
{
	glLoadIdentity();
	gluLookAt(0, 0, -1, 0, 0, 0, 0, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    initialize();
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

/**
 * The main function of the code.. 
 * The window sizes are set here.
**/
int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIND_WIDTH, WIND_HEIGHT);
    
	g_window = glutCreateWindow("Pencil Shading");
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(handleKeyboard);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glutMainLoop();
	return 0;
}
