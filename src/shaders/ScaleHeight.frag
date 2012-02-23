#version 120
// Simple scale height transparency
in vec4 color;
in float color_scale;

void main(void)
{
	// Increase transparency but leave the colors otherwise unaltered.
	gl_FragColor = vec4(color.x, color.y, color.z, color.w * color_scale);
}
