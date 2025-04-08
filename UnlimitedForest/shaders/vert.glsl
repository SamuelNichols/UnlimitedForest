#version 460 core
layout(location = 0) in vec3 vectorPosition;
layout(location = 1) in vec3 vectorColor;

uniform mat4 u_ModelMatrix;
uniform mat4 u_Perspective;

out vec3 v_vectorColor;

void main()
{
    vec4 newPosition = u_Perspective * u_ModelMatrix * vec4(vectorPosition, 1.0f);
    gl_Position = newPosition;
    v_vectorColor = vectorColor;
}
