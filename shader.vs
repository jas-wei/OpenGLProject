#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform float positionOffset;
uniform float timeValue;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 vertexColor;
out vec2 texCoord;

void main()
{
	vec2 newTexCoord = aTexCoord;
	newTexCoord -= vec2(0.5, 0.5);

	float newX = newTexCoord.x * cos(timeValue) - newTexCoord.y * sin(timeValue);
	float newY = newTexCoord.x * sin(timeValue) + newTexCoord.y * cos(timeValue);

	newTexCoord = vec2(newX, newY);
	newTexCoord += vec2(0.5f, 0.5f);

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
	texCoord = newTexCoord;
}