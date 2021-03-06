#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 objectColor;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;
void main()
{
	gl_Position = projection * view * transform * vec4(position.x,position.y,position.z,1.0f);
	objectColor = color;
	Normal = mat3(transpose(inverse(transform)))* position;
	FragPos = vec3(transform * vec4(position,1.0f));
}