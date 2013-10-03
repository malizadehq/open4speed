uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
attribute vec2 v_tid;
attribute vec2 v_tuv;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;
varying vec2 v_Tid;
varying vec2 v_Tuv;
void main()
{
    vec4 view_pos = u_ModelViewMatrix * vec4(v_vertex, 1.0);
    v_Normal = vec3(u_ModelViewMatrix * vec4(v_normal, 0.0));
    v_Vertex = view_pos.xyz;
    v_Vertex.y -= 15.0;
    v_Coords = v_coord;
    v_Tid = v_tid;
    v_Tuv = v_tuv;
    gl_Position = u_ProjectionMatrix * view_pos;
}
