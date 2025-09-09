#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in mat4 insModelMat; // instanced model matrix

out vec2 TexCoords;

uniform mat4 uProjMat;
uniform mat4 uViewMat;

uniform float uTime;             
uniform float uWindStrength;     
uniform vec3 uWindDir;

const float TREE_HEIGHT = 10.0;
const float LEAF_WIGGLE_FACTOR = 0.08f;
void main() {
    TexCoords = aTexCoords;

    // =======================
    // 🔹 Local-space leaf deformation
    // =======================

    // 葉片自體震動（在 local space 扭動 vertex）
    float leafPhase = aPos.x * 4.0 + aPos.y * 3.0 + uTime * 5.0;
    float leafWiggle = sin(leafPhase) * LEAF_WIGGLE_FACTOR; // 微小擺動幅度

    // 在 local 空間對 vertex 加上扭動位移（往法線方向偏移）
    vec3 localOffset = aNormal * leafWiggle;

    // 把頂點加上小變形
    vec4 localPos = vec4(aPos + localOffset, 1.0);

    // =======================
    // 🔹 Global wind sway
    // =======================

    vec4 worldPos = insModelMat * localPos;

    float heightFactor = clamp(worldPos.y / TREE_HEIGHT, 0.0, 1.0);
    float phase = dot(worldPos.xyz, uWindDir) * 0.5 + uTime;
    float sway = sin(phase) * uWindStrength * heightFactor;

    vec3 windOffset = uWindDir * sway;
    worldPos.xyz += windOffset;

    gl_Position = uProjMat * uViewMat * worldPos;
}
