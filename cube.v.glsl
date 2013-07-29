#version 120
attribute vec3 coord3d;
attribute vec3 v_normal;
uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
varying vec4 color;

struct lightSource
{
  vec4 position;
  vec4 diffuse;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 spotDirection;
};
lightSource light0 = lightSource(vec4(-1.0, 1.0, -1.0, 0.0),
                                 vec4(1.0, 1.0, 1.0, 1.0),
                                 0.0, 1.0, 0.0,
                                 80.0, 20.0,
                                 vec3(-1.0, -0.5, -1.0));

struct material
{
  vec4 diffuse;
};
material mymaterial = material(vec4(0.8, 0.8, 0.8, 1.0));

void main(void) {
  mat4 mvp = p*v*m;
  vec3 ml = vec3(m * vec4(1.0));
  vec3 normalDirection = normalize(m_3x3_inv_transp * v_normal);
  vec3 vertexToLightSource = vec3(light0.position) * coord3d;
  float distance = length(vertexToLightSource);
  float attenuation = 1.0 / (light0.constantAttenuation
                             + light0.linearAttenuation * distance
                             + light0.quadraticAttenuation * distance * distance);

  vec3 lightDirection = normalize(vec3(light0.position));
  vec3 ambientIntensity = vec3(0.2, 0.3, 0.35);
  vec3 transformed_light_direction = vec3(m * vec4(lightDirection, 0.0));
  vec3 diffuseReflection = attenuation * vec3(light0.diffuse) * vec3(mymaterial.diffuse) * max(0.0, dot(normalDirection, lightDirection))
                           + (vec3(light0.diffuse) * ambientIntensity);

  color = vec4(diffuseReflection, 1.0);
  gl_Position = mvp * vec4(coord3d, 1.0);
}
