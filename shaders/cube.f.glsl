#version 120

varying vec4 color;
varying vec3 normal;
varying float ambientOcclusion;

void main(void)
{
  float light = ambientOcclusion + max(1.0*dot(normal, vec3(1,1,1)), 0.0);
  gl_FragColor = vec4(color.xyz * light, 1.0);
}