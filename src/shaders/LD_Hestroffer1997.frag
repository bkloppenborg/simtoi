#version 120
// Power law limb darkening implemented according to Hestroffer (1997)
// Implemented using alpha blending.
in vec3 normal;
in vec4 color;
uniform float alpha;

void main(void)
{
    float mu = dot(normal, vec3(0.0, 0.0, 1.0));
    float intensity = pow(mu, alpha);

    gl_FragColor = vec4(color.x, color.y, color.z, color.w * intensity);
}
