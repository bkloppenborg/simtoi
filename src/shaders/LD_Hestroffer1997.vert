#version 120
// Power law limb darkening implemented according to Hestroffer (1997)
// Implemented using alpha blending.
varying out vec3 normal;
varying out vec4 color;

void main(void)
{
    normal = gl_NormalMatrix * gl_Normal;
    color = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
