#version 120
# Power law limb darkening implemented according to Hestroffer (1997)
varying vec3 normal;
varying vec4 color;

void main(void)
{
    normal = gl_NormalMatrix * gl_Normal;
    color = gl_Color;
    gl_Position = ftransform();
}
