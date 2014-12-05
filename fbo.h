#pragma once
#include <GLUT/glut.h>

class FBO
{
public:
	FBO(GLenum, GLenum, GLuint);
	void updateFBO(GLenum, GLuint);
	void updateRBO(GLint, GLint);
	void bindFBO();
	void unbindFBO();
	void bindRBO();
	void unbindRBO();
	bool inline isActive()
	{
		return active;
	}

private:
	bool activeRBO;
	bool active;
	GLuint frameBufferID;
	GLuint renderBufferID;
	GLenum attachment;
	GLenum kind;
};