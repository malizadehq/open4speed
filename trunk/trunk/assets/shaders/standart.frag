uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed;
varying vec2 v_Coords;
varying vec3 v_Vertex;
varying vec3 v_Normal;
void main()
{
  vec4 c = texture2D(color_texture, v_Coords);
  float d = dot(v_Normal, normalize(-v_Vertex));
  /*float b = (c.r + c.g + c.b) * 0.33;
  c.r *= 1.0 + (c.r - b) * 1.0;
  c.g *= 1.0 + (c.g - b) * 1.0;
  c.b *= 1.0 + (c.b - b) * 1.0;*/
  gl_FragColor = c * d;
  gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, vec2(gl_FragCoord.x, gl_FragCoord.y) * u_res); 
}
