#version 330 core
out vec4 color;
in vec3 lightColor;
void main()
{
    vec3 result = lightColor * vec3(1.0,0.4,0.4);
    color = vec4(result, 1.0f);
}