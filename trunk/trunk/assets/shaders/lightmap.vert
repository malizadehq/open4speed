uniform sampler2D color_texture;
attribute vec4 v_vertex;
attribute vec2 v_coord;
void main()
{
    vec4 color = texture2D(color_texture, v_coord);
    gl_Position.xy = (color.rg + color.ba * 0.00390625) * 2.0 - 1.0;
    gl_Position.zw = vec2(0.0, 1.0);
}
