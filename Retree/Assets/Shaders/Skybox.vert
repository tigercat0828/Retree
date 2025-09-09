#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoords;
uniform mat4 uProjMat;
uniform mat4 uViewMat;
void main()
{
    TexCoords = aPos;
    vec4 pos = uProjMat * uViewMat * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 