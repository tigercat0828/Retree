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
    
    // 預先算出 worldPos（模型矩陣後的位置）
    vec4 worldPos = uModelMat * vec4(vPos, 1.0);

    // 根據世界空間 Y 位置決定風力影響強度（Y 越高越大）
    float heightFactor = clamp(worldPos.y / TREE_HEIGHT, 0.0, 1.0);

    // 計算風的偏移（風向點積 + 時間 = 相位）
    float phase = dot(worldPos.xyz, uWindDir) * 0.5 + uTime;
    float sway = sin(phase) * uWindStrength * heightFactor;

    vec3 windOffset = uWindDir * sway;

    // 加入偏移
    worldPos.xyz += windOffset;

    gl_Position = uProjMat * uViewMat * worldPos;
}
