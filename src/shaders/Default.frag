#version 120
// Default (do nothing) fragement shader.
varying in vec4 color;

void main(void)
{
    gl_FragColor = color;
}
