#include <iostream>
#include <cstdlib>
#include "bmp.h"
#include "fbo.h"
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

Shader nprShader, curvShader, zBufShader;
Texture zBufTexture, pencilTexture, curvTexture;
TriMesh * mesh;
FBO *curvatureFBO;

float texCoordOffSets[18];

void populateTexCoordOffSets()
{
    GLfloat dx = 0.3f / (GLfloat)WIND_WIDTH;
    GLfloat dy = 0.3f / (GLfloat)WIND_HEIGHT; 

    for(int x=0; x<3; x++)
    {
        for(int y=0; y<3; y++)
        {
            int index = ((x*3)+y)*2;
            texCoordOffSets[index] =  (dx * -1.0) + (dx * (GLfloat)x);
            texCoordOffSets[index+1] = (dy * -1.0) + (dy * (GLfloat)y);
        }
    }
}

void drawImage()
{
    curvatureFBO->bindFBO();
    curvShader.enable();
    glColor3ub(255, 255, 255);
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
            curvShader.setAttribute("maxCurvDir", mesh->pdir1[index][0], mesh->pdir1[index][1], mesh->pdir1[index][2]);
            curvShader.setAttribute("minCurvDir", mesh->pdir2[index][0], mesh->pdir2[index][1], mesh->pdir2[index][2]);
            //std::cout<<index<<" : ";
            //std::cout<<mesh->pdir1[index]<<"\n";
        }
        glEnd();
    } 
    curvShader.disable();
    curvatureFBO->unbindFBO();
}

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
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glShadeModel(GL_LIGHT0);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
}

void drawTexture()
{
    glEnable(GL_TEXTURE_3D);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, zBufTexture.getID());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, zBufTexture.getID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pencilTexture.getID());
   
    nprShader.enable();
    nprShader.setUniform("srcTexture", 0);
    nprShader.setUniform("pencilTexture", 1);
    nprShader.setUniform("curvTexture", 2);
    nprShader.setUniform("offset", 9, texCoordOffSets);

    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
   // glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd(); 
    glDisable(GL_TEXTURE_3D);
    nprShader.disable(); 
}

void initialize()
{
    populateTexCoordOffSets();
    initializeLightValues();

    zBufShader.setShader("zBuf", "zBuf");
    nprShader.setShader("npr", "npr");
    curvShader.setShader("curv", "curv");


    zBufTexture.init(GL_RGBA8, WIND_WIDTH, WIND_HEIGHT);
    curvTexture.init(GL_RGBA8, WIND_WIDTH, WIND_HEIGHT);
    curvatureFBO = new FBO(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, curvTexture.getID());
    curvatureFBO->updateRBO(WIND_WIDTH, WIND_HEIGHT);

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
    /*for(int i=0; i<curvSize; i++)
    {
        std::cout<<"Point "<<i+1<<"\t";
        std::cout<<"Curvature1 "<<mesh->curv1[i]<<"\t";
        std::cout<<"Curvature2 "<<mesh->curv2[i]<<"\n";
        //std::cout<<"Normal"<<mesh->normals[i]<<"\n\n";
    }*/
    //std::cout<<nf;
    //std::cout<<nv;
}

void drawScene()
{
    readFromMesh("cube.obj");
    
    //glOrtho(-512, 512, -512, 512, -200, 200);
    glOrtho(-2, 2, -2, 2, -2, 2);
    drawImage();
    drawTexture();
    //
  /*  glEnable(GL_TEXTURE_3D);
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
    glBegin(GL_QUADS);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,1,-1);
    glVertex3f(1,1,-1);
    glVertex3f(1,-1,-1);
    glEnd();
    glDisable(GL_TEXTURE_3D); */
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
    //drawImage();
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
