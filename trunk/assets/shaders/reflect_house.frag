uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed;
varying vec3 v_Normal;
varying vec3 v_Vertex;
varying vec2 v_Coords;
void main()
{
  vec4 c = texture2D(color_texture, v_Coords);
  /*float b = (c.r + c.g + c.b) * 0.33;
  c.r *= 1.0 + (c.r - b) * 2.0;
  c.g *= 1.0 + (c.g - b) * 2.0;
  c.b *= 1.0 + (c.b - b) * 2.0;*/
  gl_FragColor = c * max(dot(v_Normal, normalize(-v_Vertex)), 0.5) * 2.0;
  gl_FragColor = c.a < 0.8 ? gl_FragColor + 0.5 * texture2D(EnvMap1, vec2(v_Normal.x * 0.5 + 0.5, gl_FragCoord.y * u_res)) : gl_FragColor;
  gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, vec2(gl_FragCoord.x, gl_FragCoord.y) * u_res); 
}
