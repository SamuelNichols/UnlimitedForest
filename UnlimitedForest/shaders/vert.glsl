#version 460 core
layout(location = 0) in vec3 vectorPosition;
layout(location = 1) in vec3 vectorColor;

out vec3 v_vectorColor;

void main()
{
    gl_Position = vec4(vectorPosition, 1.0f);
    v_vectorColor = vectorColor;
}
