#include <iostream>
#include <cstdlib>
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

/**
 * Method to initialize the lighting values in the scene.
**/
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

/**
 * This is a hack kind of a situation where we want to render all kinds of obj file meshes, 
 * and to ensure that the different objects fit inside the screen space, 
 * considering that the vertices are all different inside the object space,
 * we normalize the screen coordinates accordingly to accomodate most of the objects.
**/
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

/**
 * Reading the obj file through the trimesh code to retrieve the mesh information.
 * Also, enabling it to find the curvatures at each vertex.
**/
void readFromMesh(char * filename)
{
    mesh = TriMesh::read(filename);
    if(!mesh)
        exit(1);
    mesh->need_curvatures();
}

/**
 * Method to initialize the shader objects and the texture objects
**/
void initShaderObjects()
{
    // Initializing all the shader objects
    zBufShader.setShader("zBuffer", "zBuffer");
    nprShader.setShader("npr", "npr");
    curvShader.setShader("curv", "curv");

    // Initializing all the texture objects
    zBufTexture.init(GL_RGBA16F, WIND_WIDTH, WIND_HEIGHT);
    curvTexture.init(GL_RGBA16F, WIND_WIDTH, WIND_HEIGHT);
    
    // Initializing all the framebuffer objects
    curvatureFBO = new FBO(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, curvTexture.getID());
    curvatureFBO->updateRBO(WIND_WIDTH, WIND_HEIGHT);
    zBufFBO = new FBO(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, zBufTexture.getID());
    zBufFBO->updateRBO(WIND_WIDTH, WIND_HEIGHT);
}


/**
 * Method to read the pencil strokes from the bmp file, and store it as a texture object.
**/
void readFromPicture()
{
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

/**
 * Method to populate the z buffer texture through the z buffer frame buffer object.
 * In this pass, we compute the object material shading values for each vertex of the object.
**/
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

/**
 * Method to populate the curvature texture through the curvature frame buffer object
 * In this pass, at each vertex, we get the curvature directions, which is to be used for rotation of texture later in the pipeline.
**/
void populateCurvatureFBO()
{
    curvShader.enable();
    curvatureFBO->bindFBO();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
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
            curvShader.setAttribute("curvatureDirection", mesh->pdir1[index][0], mesh->pdir1[index][1], mesh->pdir1[index][2]);
            curvShader.setAttribute("minCurvatureDirection", mesh->pdir2[index][0], mesh->pdir2[index][1], mesh->pdir2[index][2]);
        }
        glEnd();
    }
    curvatureFBO->unbindFBO();
    curvShader.disable();
}

/**
 * The initialize function
 * First, we initialize all the shader objects, along with the frame buffer objects mapped with their corresponding texture objects.
 * Then, we initialize the lighting and material color values.
 * Then, we initialize the mesh object from the obj file with all the vertices and normal values from the trimesh structure
 * Lastly, we compute the boundary points of the object on the screen, thereby comptuing the view matrix, glOrtho, etc to ensure that the object coordinates fit onto the screen.
**/
void initialize()
{
    initShaderObjects();
    initializeLightValues();
    readFromMesh("granade.obj");
    computeBoundaryPoints(); 
}

/**
 * The final step of rendering where the pencil texture is mapped onto the screen.
 * From the other two textures, we compute the exact points on the screen which needs to be colored.
**/
void drawTexture()
{
    // Binding all the texture object values
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, curvTexture.getID());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, zBufTexture.getID());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tempTexture.getID());
   
    // Enabling the npr shader with all the uniform texture attributes
    nprShader.enable();
    nprShader.setUniform("texSrc", 0);
    nprShader.setUniform("texCur", 2);
    nprShader.setUniform("tempTex", 3);

    // Mapping the texture onto the whole of screen
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(xMin, yMin);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(xMax, yMin);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(xMax, yMax);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(xMin, yMax);
    glEnd(); 
    
    // Disabling the texture at the end
    nprShader.disable();
}

/**
 * The method to draw the scene objects.
 * First, we take in the pencil strokes from the bmp file and then store in a texture object
 * Then, we render the object into a frame buffer object. Its in here that we apply the shading on the object. This object is mapped to a zBufTexture
 * Then, we render the object into a curvature frame buffer object. Its in here that we store the curvature directions required for rotation of texture imagges. This image is mapped to a curvTexture object
 * We take all the three textures and map them onto the screen, thereby rendering the pencil shading.
**/
void drawScene()
{
    glOrtho(xMin, xMax, yMin, yMax, zMin, zMax);
    readFromPicture();
    populateZBufferFBO();
    populateCurvatureFBO();
    drawTexture();
}

/**
 * Display function of the code
**/
void renderScene()
{
	glLoadIdentity();
	gluLookAt(0, 0, -1, 0, 0, 0, 0, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    initialize();
	drawScene();
    glutSwapBuffers();
}

/**
 * Method which deals with all the keyboard press actions
**/
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
