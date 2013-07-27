#version 120
attribute vec3 coord3d;
attribute vec3 v_normal;
uniform mat4 mvp;
uniform mat3 m_3x3_inv_transp;
varying vec4 color;

struct lightSource
{
  vec4 position;
  vec4 diffuse;
};
lightSource light0 = lightSource(vec4(-1.0, 1.0, -1.0, 0.0),
                                 vec4(1.0, 1.0, 1.0, 1.0)
);

struct material
{
  vec4 diffuse;
};
material mymaterial = material(vec4(1.0, 0.8, 0.8, 1.0));

void main(void) {
  vec3 normalDirection = normalize(m_3x3_inv_transp * v_normal);
  vec3 lightDirection = normalize(vec3(light0.position));

  vec3 diffuseReflection = vec3(light0.diffuse) * vec3(mymaterial.diffuse) * max(0.0, dot(normalDirection, lightDirection));

  color = vec4(diffuseReflection, 1.0);
  gl_Position = mvp * vec4(coord3d, 1.0);
}
