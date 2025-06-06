#version 460 core
in vec3 v_vectorColor;

uniform mat4 u_ModelMatrix;
uniform mat4 u_Perspective;

out vec4 FragColor;

void main()
{
    FragColor = vec4(v_vectorColor.r, v_vectorColor.g, v_vectorColor.b, 1.0f);
}