#version 120
// Default (do nothing) shader.

void main() 
{ 
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
