uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed;
varying vec2 v_Coords;
void main()
{
    vec4 c = texture2D(color_texture, v_Coords);
    if (c.a < 0.9) {
	discard;
    }
    gl_FragColor = c * 2.0;
    gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, vec2(gl_FragCoord.x, gl_FragCoord.y) * u_res); 
}
