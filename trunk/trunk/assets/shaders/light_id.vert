uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
attribute vec2 v_tid;
attribute vec2 v_tuv;
varying vec2 v_Coords;
varying vec2 v_Tid;
varying vec2 v_Tuv;

void main()
{
    v_Coords = v_coord;
    v_Tid = v_tid;
    v_Tuv = v_tuv;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
