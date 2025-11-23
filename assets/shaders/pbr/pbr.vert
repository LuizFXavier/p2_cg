#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 mvMatrix;
uniform mat3 nMatrix;
uniform mat4 mvpMatrix;

out vec3 vPos;
out vec3 vNormal;

void main() {

    vPos = vec3(mvMatrix * vec4(position, 1.0));
    
    vNormal = normalize(nMatrix * normal);

    gl_Position = mvpMatrix * vec4(position, 1.0);

}