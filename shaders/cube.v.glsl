#version 120

attribute vec3 v_coord;
attribute vec3 data;
uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
uniform mat4 v_inv;
varying vec4 color;
varying vec3 normal;
varying float ambientOcclusion;

struct lightSource
{
  vec4 position;
  vec4 diffuse;
  vec4 specular;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 spotDirection;
};
lightSource light0 = lightSource(
  vec4(0.0,  1.0,  2.0, 1.0),
  vec4(1.0,  1.0,  1.0, 1.0),
  vec4(1.0,  1.0,  1.0, 1.0),
  0.0, 1.0, 0.0,
  180.0, 0.0,
  vec3(-1.0, 0.0, 0.0)
);
vec4 scene_ambient = vec4(0.4, 0.4, 0.4, 1.0);

struct material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
material mymaterial = material(
  vec4(0.8, 0.0, 0.0, 1.0),
  vec4(0.4, 0.4, 0.4, 1.0),
  vec4(1.0, 1.0, 1.0, 1.0),
  5.0
);

void main(void)
{
  ambientOcclusion = data.y / 255.0;
  float side = data.x;
  if(side == 0) {
    normal = vec3(1.0, 0.0, 0.0);
  } else if(side == 1) {
    normal = vec3(0.0, 1.0, 0.0);
  } else if(side == 2) {
    normal = vec3(0.0, 0.0, 1.0);
  } else if(side == 3) {
    normal = vec3(-1.0, 0.0, 0.0);
  } else if(side == 4) {
    normal = vec3(0.0, -1.0, 0.0);
  } else if(side == 5) {
    normal = vec3(0.0, 0.0, -1.0);
  }

  mat4 mvp = p*v*m;
  vec3 normalDirection = normalize(m_3x3_inv_transp * normal);
  vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - m * vec4(v_coord, 1.0)));
  vec3 lightDirection;
  float attenuation;

  if (light0.position.w == 0.0) // directional light
    {
      attenuation = 1.0; // no attenuation
      lightDirection = normalize(vec3(light0.position));
    }
  else // point or spot light (or other kind of light)
    {
      vec3 vertexToLightSource = vec3(light0.position - m * vec4(v_coord, 1.0));
      float distance = length(vertexToLightSource);
      lightDirection = normalize(vertexToLightSource);
      attenuation = 1.0 / (light0.constantAttenuation
                           + light0.linearAttenuation * distance
                           + light0.quadraticAttenuation * distance * distance);

      if (light0.spotCutoff <= 90.0) // spotlight
        {
          float clampedCosine = max(0.0, dot(-lightDirection, normalize(light0.spotDirection)));
          if (clampedCosine < cos(radians(light0.spotCutoff))) // outside of spotlight cone
            {
              attenuation = 0.0;
            }
          else
            {
              attenuation = attenuation * pow(clampedCosine, light0.spotExponent);
            }
        }
    }

  vec3 ambientLighting = vec3(scene_ambient) * vec3(mymaterial.ambient);

  vec3 diffuseReflection = attenuation
    * vec3(light0.diffuse) * vec3(mymaterial.diffuse)
    * max(0.0, dot(normalDirection, lightDirection));

  vec3 specularReflection;
  if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
    {
      specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
    }
  else // light source on the right side
    {
      specularReflection = attenuation * vec3(light0.specular) * vec3(mymaterial.specular)
        * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)),
              mymaterial.shininess);
    }

  color = vec4(ambientLighting + diffuseReflection + specularReflection, 1.0);
  gl_Position = mvp * vec4(v_coord, 1.0);
}