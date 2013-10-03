uniform sampler2D color_texture;
varying vec2 v_Coords;
varying vec2 v_Tid;
varying vec2 v_Tuv;

void main()
{
    vec4 c = texture2D(color_texture, v_Coords);
    if (c.a < 0.9) {
	discard;
    }
    gl_FragColor = vec4(v_Tid.s, v_Tid.t, v_Tuv.s, v_Tuv.t);
}
