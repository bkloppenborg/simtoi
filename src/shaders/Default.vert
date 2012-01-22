#version 120
// Default (do nothing) shader.
varying out vec4 color;

void main() 
{ 
    color = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
