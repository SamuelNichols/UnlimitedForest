#version 460 core
layout(location = 0) in vec3 vectorPosition;
layout(location = 1) in vec3 vectorColor;

uniform float u_YOffset;
uniform float u_XOffset;

out vec3 v_vectorColor;

void main()
{
    gl_Position = vec4(vectorPosition.x + u_XOffset, vectorPosition.y + u_YOffset,vectorPosition.z, 1.0f);
    v_vectorColor = vectorColor;
}
