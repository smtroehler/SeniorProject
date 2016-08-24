#ifndef __Texture__
#define __Texture__

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif
#ifdef _WIN32



#include <Windows.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_GLCOREARB
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <string>

class Texture
{
public:
	Texture();
	virtual ~Texture();
	void setFilename(const std::string &f) { filename = f; }
	bool init();
	void bind(GLint handle, GLint unit);
	void unbind();
	std::string getName() {return std::string(filename);}
private:
	std::string filename;
	int width;
	int height;
	GLuint tid;
	GLint unit;
	
};

#endif
