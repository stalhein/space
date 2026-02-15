#version 300 es
precision highp float;
precision highp int;

layout(location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uModel;

out vec3 vFragPos;
out vec3 vNormal;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);

    vFragPos = worldPos.xyz;

    gl_Position = uProjection * uView * worldPos;

    vNormal = mat3(transpose(inverse(uModel))) * aPos;
}