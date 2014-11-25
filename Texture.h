#include <GLUT/glut.h>

class Texture
{
public:
	Texture(void);
	~Texture(void);
	void init(GLenum tempFormat, GLint width, GLint height);
	void init(GLenum tempFormat, GLint width, GLint height, GLubyte *data);
	void init(GLenum tempFormat, GLint width, GLint height, GLint depth, GLubyte *data);
	GLuint inline getID()
	{
		return textureID;
	}
private:
	bool isActive;
	GLenum format;
	GLuint textureID;

};