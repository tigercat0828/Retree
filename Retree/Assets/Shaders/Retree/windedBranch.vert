#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec2 fTexCoord;
out vec3 fNormal;

uniform mat4 uModelMat;
uniform mat4 uViewMat = mat4(1.0);
uniform mat4 uProjMat = mat4(1.0);

uniform float uTime;
uniform float uWindStrength;
uniform vec3 uWindDir;

const float TREE_HEIGHT = 10.0f;

void main() {
    fTexCoord = vTexCoord;
    
    // �w����X worldPos�]�ҫ��x�}�᪺��m�^
    vec4 worldPos = uModelMat * vec4(vPos, 1.0);

    // �ھڥ@�ɪŶ� Y ��m�M�w���O�v�T�j�ס]Y �V���V�j�^
    float heightFactor = clamp(worldPos.y / TREE_HEIGHT, 0.0, 1.0);

    // �p�⭷�������]���V�I�n + �ɶ� = �ۦ�^
    float phase = dot(worldPos.xyz, uWindDir) * 0.5 + uTime;
    float sway = sin(phase) * uWindStrength * heightFactor;

    vec3 windOffset = uWindDir * sway;

    // �[�J����
    worldPos.xyz += windOffset;

    gl_Position = uProjMat * uViewMat * worldPos;
}
