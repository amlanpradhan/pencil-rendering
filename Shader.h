#include <GLUT/glut.h>

class Shader
{
public:
	Shader(void);
	~Shader(void);
	void setShader(const char *, const char *);
	GLuint shaderProgram;

private:
	char *shaderFileRead(const char *fRead);
	GLuint vertShader;
	GLuint fragShader;
};
