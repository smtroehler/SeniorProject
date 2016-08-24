all:
	g++ -g -o fp -std=c++11 -I /usr/local/include/  *.cpp *.cc -DGL_GLEXT_PROTOTYPES -lglut  -lGL -lGLU