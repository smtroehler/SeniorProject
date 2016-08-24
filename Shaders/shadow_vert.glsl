#version 330 core


// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;
in vec4 vertPos; // in object space

void main() {
   gl_Position = vertPos;
}