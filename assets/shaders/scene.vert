uniform float u_res;
varying vec2 texture_coordinate;
varying vec2 texture_coordinateNW;
varying vec2 texture_coordinateNE;
varying vec2 texture_coordinateSW;
varying vec2 texture_coordinateSE;
attribute vec4 v_vertex;
attribute vec2 v_coord;
void main()
{
    texture_coordinate = v_coord.st;
    texture_coordinateNW = v_coord.st + vec2(-u_res, -u_res);
    texture_coordinateNE = v_coord.st + vec2(+u_res, -u_res);
    texture_coordinateSW = v_coord.st + vec2(-u_res, +u_res);
    texture_coordinateSE = v_coord.st + vec2(+u_res, +u_res);
    gl_Position = v_vertex;
}
