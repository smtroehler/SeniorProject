#version 330 core

in vec4 vertPos; // in object space
in vec2 vertTex;

uniform mat4 P;
uniform mat4 MV;
uniform vec2 UVScale;
uniform mat4 depthBiasMVP;

out vec2 fragTex;
out vec4 shadowCoord;


void main()
{
   gl_Position = P * MV * vertPos;
   shadowCoord = depthBiasMVP * vertPos;
   fragTex = vec2(vertTex.x * UVScale.x, vertTex.y * UVScale.y);
}



