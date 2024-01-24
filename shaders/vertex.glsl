#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 projection;

out vec4 vertexColor;

void main()
{
    gl_Position = projection * vec4(position.x, position.y, position.z, 1.0);
    gl_PointSize = 7;
    vertexColor = vec4(color, 1.0);
}
