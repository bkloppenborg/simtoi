#version 120
// Simple scale height transparency.
varying out vec4 color;
varying out float color_scale;
uniform float h_z;
uniform float max_z;

void main(void)
{
    color = gl_Color;
    color_scale = exp(- (max_z - abs(gl_Vertex.z))/h_z);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
