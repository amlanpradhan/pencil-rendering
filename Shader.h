#include <GLUT/glut.h>

class Shader
{
public:
	Shader(void);
	~Shader(void);
	void setShader(const char *, const char *);
	void enable();
	void disable();
	void setUniform(const GLchar *uniformName, GLint uniformVal);
	void setUniform(const GLchar *uniformName, GLint uniformVal, GLfloat *x);
	void setAttribute(const GLchar *uniformName, GLfloat x, GLfloat y, GLfloat z);
	GLuint shaderProgram;

private:
	char *shaderFileRead(const char *fRead);
	GLuint vertShader;
	GLuint fragShader;
};
