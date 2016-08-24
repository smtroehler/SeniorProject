#version 330 core
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;


uniform float inten;

in vec3 fragNor;
in vec3 fragPos;
in vec2 fragTex;

in vec3 ligh1;

uniform sampler2D texture0;
uniform int highlighted;

out vec4 fragColor;

void main()
{
   vec3 n = normalize(fragNor);
   vec3 finalColor = vec3(0.0,0.0,0.0);
   vec4 tex = texture2D(texture0, fragTex).rgba;
   vec3 color_a;
   color_a.r = ka.r * tex.r;
   color_a.g = ka.g * tex.g;
   color_a.b = ka.b * tex.b;


   vec3 l = normalize(ligh1 - fragPos); 
   vec3 color_d  = tex.rgb * max(dot(l, n), 0);
   vec3 e = normalize(-fragPos);
   vec3 h = normalize(l + e); 
   vec3 color_s = tex.rgb * tex.a * pow(max(dot(h, n), 0), s);
   color_s.r *= ks.r;
   color_s.g *= ks.g;
   color_s.b *= ks.b;
   vec3 color = color_a + color_d +  color_s;
   finalColor = inten * color;
   
   if(highlighted != 0)
   {
      finalColor = vec3(finalColor.r * 3, finalColor.g / 3, finalColor.b / 3);
   }
   fragColor = vec4(finalColor, 1.0);

}
