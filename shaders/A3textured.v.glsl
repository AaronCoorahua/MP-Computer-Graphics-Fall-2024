#version 410 core
layout (location = 0) in vec3 vPos;

out vec3 texCoords;

uniform mat4 mvpMatrix;

void main()
{
    vec4 pos = mvpMatrix * vec4(vPos, 1.0);
    // Having z equal w will always result in a depth of 1.0f
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);

    // Flip the z axis due to the different coordinate systems (left hand vs right hand)
    texCoords = vec3(vPos.x, vPos.y, -vPos.z);

}