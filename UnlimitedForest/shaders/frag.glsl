#version 460 core
in vec3 v_vectorColor;

uniform float u_YOffset;
uniform float u_XOffset;

out vec4 FragColor;

void main()
{
    FragColor = vec4(v_vectorColor.r - u_XOffset, v_vectorColor.g + u_YOffset, v_vectorColor.b, 1.0f);
}