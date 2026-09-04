#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPos;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 TexCoords;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

    worldPosition = vec3(modelMatrix * vec4(aPos, 1.0));
    worldNormal = vec3(transpose(inverse(modelMatrix)) * vec4(aNormal, 0.0));

    TexCoords = aTexCoords;
}