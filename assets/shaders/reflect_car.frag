uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed, u_view, u_test;
varying vec3 v_Vertex2;
varying vec3 v_Normal;
varying vec2 v_Coords;
void main()
{
  gl_FragColor = texture2D(color_texture, v_Coords);
  gl_FragColor += 2.0 * texture2D(EnvMap1, vec2(0.5 + v_Normal.x * 0.25, 1.5 - gl_FragCoord.z * 1.0 + v_Normal.y * 0.25) * u_view) * clamp(0.0, (v_Vertex2.y - 0.4), 1.0);
  gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, vec2(gl_FragCoord.x, gl_FragCoord.y) * u_res); 
}
