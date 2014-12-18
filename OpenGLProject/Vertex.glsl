#version 110
attribute vec4 v_coord;
attribute vec3 v_normal;

varying vec3 N;
varying vec3 v;
varying vec4 position;  // position of the vertex (and fragment) in world space
varying vec3 varyingNormalDirection;  // surface normal vector in world space
uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
 
void main()
{
  position = m * v_coord;
  varyingNormalDirection = normalize(m_3x3_inv_transp * v_normal);
  v = vec3(m* v * v_coord);       
   N = normalize(gl_NormalMatrix * v_normal);
 
  mat4 mvp = p*v*m;
  gl_Position = mvp * v_coord;
}

