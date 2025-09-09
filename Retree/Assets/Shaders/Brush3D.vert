#version 460 core
layout(location =0) in vec3 vPos;

uniform mat4 uViewMat;
uniform mat4 uProjMat;

void main(){
	gl_Position =uProjMat * uViewMat *  vec4(vPos, 1.0f);
}