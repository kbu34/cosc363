#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

out vec2 vert_texCoord;

uniform mat4 mvpMatrix;

void main()
{
   gl_Position = mvpMatrix * position;
   vert_texCoord = texCoord;
   
}
