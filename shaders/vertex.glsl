#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uModel;

out vec3 vNormal;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);


    gl_Position = uProjection * uView * worldPos;

    vNormal = mat3(transpose(inverse(uModel))) * aPos;
}