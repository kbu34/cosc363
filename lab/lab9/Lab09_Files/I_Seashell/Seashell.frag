#version 330

in vec4 oColor;
in float diffTerm;
in float nv;

out vec4 outputColor;

void main()
{
   if (nv < 0.2) {
      outputColor = vec4(0.0);
   } else if (diffTerm < 0.2) {
      outputColor = vec4(0.2);
   } else {
      outputColor = vec4(0.9);
   }
   
}
