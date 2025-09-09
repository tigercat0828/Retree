#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in mat4 insModelMat;	// instanced


out vec2 TexCoords;

uniform mat4 uProjMat;
uniform mat4 uViewMat;

void main(){
	TexCoords = aTexCoords;
	gl_Position = uProjMat * uViewMat * insModelMat * vec4(aPos ,1.0);
}