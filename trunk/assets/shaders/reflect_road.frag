uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed, u_view;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;
void main()
{
  gl_FragColor = texture2D(color_texture, v_Coords) * 1.5;
  gl_FragColor += 0.5 * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_res, 4.0 + u_view - gl_FragCoord.y * u_res - 0.2 + gl_FragCoord.z * (u_view * 10.0 + 5.0) * 0.025 - v_Normal.y * 4.0)) * max(gl_FragCoord.y * u_res / u_view * 2.0 - 0.5, 0.0);
  gl_FragColor *= dot(v_Normal, normalize(-v_Vertex));
  gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, vec2(gl_FragCoord.x, gl_FragCoord.y) * u_res); 
}
