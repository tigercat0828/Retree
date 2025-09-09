#version 460 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec2 fTexCoord;
out vec3 fNormal;

uniform mat4 uModelMat;
uniform mat4 uViewMat = mat4(1.0);
uniform mat4 uProjMat = mat4(1.0);

void main() {
	gl_Position = uProjMat * uViewMat * uModelMat * vec4(vPos, 1.0);

    //gl_Position = ProjMatrix * ViewMatrix * uModelMat * vec4(vPos, 1.0);
    fTexCoord = vTexCoord;
}
