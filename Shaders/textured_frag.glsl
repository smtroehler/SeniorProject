#version 330 core

uniform sampler2D texture0;
uniform sampler2D shadowMap;
in vec2 fragTex;

out vec4 color;
uniform int highlighted;
in vec4 shadowCoord;
void main()
{
  // vec3 tex = vec3(0,0,0); //texture2D(texture0, fragTex).rgb ;
  float red = texture2D(shadowMap, fragTex).a;
  
   vec3 tex = vec3(0,0,0);
   tex.r = tex.g = tex.b = red;

  // float visiblity = texture(shadowMap, vec3(shadowCoord.xyz));

//   if ( texture( shadowMap, shadowCoord.xy ).z  <  shadowCoord.z){
//         visibility = 0.5;
 //  }

   if(highlighted != 0)
   {
      tex = vec3(tex.r * 3, tex.g / 3, tex.b / 3);
   }
   color = vec4(tex, 1.0);
}