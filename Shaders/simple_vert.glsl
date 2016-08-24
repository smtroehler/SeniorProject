#version 330 core
in vec4 vertPos; // in object space

uniform mat4 P;
uniform mat4 MV;

uniform vec4 color;

out vec4 fragcolor;
void main()
{
   gl_Position = P * MV * vertPos;
   fragcolor = vec4(.5,.5,.5,.5);
}