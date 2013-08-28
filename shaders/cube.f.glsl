#version 120

varying vec4 color;
varying vec4 normal;

void main(void)
{
  float light = normal.w + max(0.15*dot(normal.xyz, vec3(1,1,1)), 0.0);
  gl_FragColor = vec4(color.xyz * light, 1.0);
}