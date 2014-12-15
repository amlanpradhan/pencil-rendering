#include <GLUT/glut.h> // Include the GLUT header file  
#include <iostream> // Allow us to print to the console
#include "trimesh2/Trimesh.h"
#include "bmp.h"
#include "Shader.h"

using namespace trimesh;

TriMesh * mesh;  
Shader nprShader;
  
bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)  
  
unsigned int fbo; // The frame buffer object  
unsigned int fbo_depth; // The depth buffer for the frame buffer object  
unsigned int fbo_texture; // The texture object to write our frame buffer object to  


GLubyte tempImage[512][512][4];

unsigned int tex_obj;
  
int window_width = 500; // The width of our window  
int window_height = 500; // The height of our window  
  
float rotation_degree = 0.0f; // The angle of rotation in degrees for our teapot  

void loadTextureFromImage()
{
	BMPLoader *bmpLoader = new BMPLoader();
    char filename[100];
    sprintf(filename, "texture/texture14.bmp");
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

	glGenTextures(1, &tex_obj); // Generate one texture  
	glBindTexture(GL_TEXTURE_2D, tex_obj); // Bind the texture fbo_texture  
  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tempImage[0][0][0]); // Create a standard texture with the width and height of our window  
  
	// Setup the basic texture parameters  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
  
	// Unbind the texture  
	glBindTexture(GL_TEXTURE_2D, 0);      
}
  
void initFrameBufferDepthBuffer(void) {  
  
glGenRenderbuffersEXT(1, &fbo_depth); // Generate one render buffer and store the ID in fbo_depth  
glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_depth); // Bind the fbo_depth render buffer  
  
glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, window_width, window_height); // Set the render buffer storage to be a depth component, with a width and height of the window  
  
glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo_depth); // Set the render buffer of this buffer to the depth buffer  
  
glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0); // Unbind the render buffer  
}  
  
void initFrameBufferTexture(void) {  
glGenTextures(1, &fbo_texture); // Generate one texture  
glBindTexture(GL_TEXTURE_2D, fbo_texture); // Bind the texture fbo_texture  
  
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // Create a standard texture with the width and height of our window  
  
// Setup the basic texture parameters  
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
  
// Unbind the texture  
glBindTexture(GL_TEXTURE_2D, 0);  
}  
  
void initFrameBuffer(void) {  
initFrameBufferDepthBuffer(); // Initialize our frame buffer depth buffer  
  
initFrameBufferTexture(); // Initialize our frame buffer texture  
  
glGenFramebuffersEXT(1, &fbo); // Generate one frame buffer and store the ID in fbo  
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo); // Bind our frame buffer  
  
glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo_texture, 0); // Attach the texture fbo_texture to the color buffer in our frame buffer  
  
glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo_depth); // Attach the depth buffer fbo_depth to our frame buffer  
  
GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); // Check that status of our generated frame buffer  
  
if (status != GL_FRAMEBUFFER_COMPLETE_EXT) // If the frame buffer does not report back as complete  
{  
std::cout << "Couldn't create frame buffer" << std::endl; // Output an error to the console  
exit(0); // Exit the application  
}  
  
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind our frame buffer  
}  
  
void init(void) {  
glEnable(GL_TEXTURE_2D); // Enable texturing so we can bind our frame buffer texture  
glEnable(GL_DEPTH_TEST); // Enable depth testing  
  
initFrameBuffer(); // Create our frame buffer object  
mesh = TriMesh::read("granade.obj");
loadTextureFromImage();
nprShader.setShader("sample", "sample");
}  
  
void drawObject()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo); 
	glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT); 
	glViewport(0, 0, window_width, window_height); 
	glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glLoadIdentity();
	glTranslatef(0.0f, -5.0f, -30.0f);
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
        }
        glEnd();
    }
       // glutSolidTeapot(1.0f); 
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); 
}
 
void display (void) { 
 
//renderTeapotScene(); // Render our teapot scene into our frame buffer 
drawObject(); 

glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Clear the background of our window to red 
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the colour buffer (more buffers later on) 
glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations 
 
glTranslatef(0.0f, 0.0f, -1.0f); 

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, fbo_texture); // Bind our frame buffer texture 
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, tex_obj);
nprShader.enable();
nprShader.setUniform("fbo_tex", 0);
nprShader.setUniform("pencil_tex", 1);
//glBindTexture(GL_TEXTURE_2D, fbo_texture); 
glBegin(GL_QUADS); 
 
glTexCoord2f(0.0f, 0.0f); 
glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner 
 
glTexCoord2f(0.0f, 1.0f); 
glVertex3f(-1.0f, 1.0f, 0.0f); // The top left corner 
 
glTexCoord2f(1.0f, 1.0f); 
glVertex3f(1.0f, 1.0f, 0.0f); // The top right corner 
 
glTexCoord2f(1.0f, 0.0f); 
glVertex3f(1.0f, -1.0f, 0.0f); // The bottom right corner 
 
glEnd(); 
 nprShader.disable();
glBindTexture(GL_TEXTURE_2D, 0); // Unbind any textures 
glutSwapBuffers(); 
} 
 
void reshape (int width, int height) { 
glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window 
glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed 
glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)  
gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes  
glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly  
}  
  
static void handleKeyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        default:
        	break;
        case 27:
        	//glutDestroyWindow(g_window);
        	exit(0);
        	break;
    }
}
  
int main (int argc, char **argv) {  
glutInit(&argc, argv); // Initialize GLUT  
glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA); // Set up a basic display buffer (only single buffered for now)  
glutInitWindowSize (500, 500); // Set the width and height of the window  
glutInitWindowPosition (100, 100); // Set the position of the window  
glutCreateWindow ("Your first OpenGL Window"); // Set the title for the window  
   
  
init();  
  
glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering  
glutIdleFunc(display); // Tell GLUT to use the method "display" for rendering  
  
glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping  
  
glutKeyboardFunc(handleKeyboard); // Tell GLUT to use the method "keyPressed" for key presses  
  
glutMainLoop(); // Enter GLUT's main loop  
}