#version 330 core

uniform sampler2D texture0;
in vec2 fragTex;

out vec4 color;

void main()
{
   vec3 tex = texture2D(texture0, fragTex).rgb;

   color = vec4(tex, 1.0);
}
