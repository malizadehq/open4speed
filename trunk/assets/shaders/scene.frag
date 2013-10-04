uniform sampler2D color_texture;
uniform float u_res;
varying vec2 texture_coordinate;
varying vec2 texture_coordinateNW;
varying vec2 texture_coordinateNE;
varying vec2 texture_coordinateSW;
varying vec2 texture_coordinateSE;

const vec3 luma = vec3(0.299, 0.587, 0.114);

void main() {
//  gl_FragColor = texture2D(color_texture, texture_coordinate);
  vec3 colorNW = texture2D(color_texture, texture_coordinateNW).xyz;
  vec3 colorNE = texture2D(color_texture, texture_coordinateNE).xyz;
  vec3 colorSW = texture2D(color_texture, texture_coordinateSW).xyz;
  vec3 colorSE = texture2D(color_texture, texture_coordinateSE).xyz;

  float lumaNW = dot(colorNW, luma);
  float lumaNE = dot(colorNE, luma);
  float lumaSW = dot(colorSW, luma);
  float lumaSE = dot(colorSE, luma);
	
  float u = -lumaNW + lumaSW - lumaNE + lumaSE;
  float v =  lumaNW + lumaSW - lumaNE - lumaSE;
  vec2 dir = vec2(u, v) * 0.5 / (min(abs(u), abs(v)) + max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.03125, 0.0078125)) * u_res;
  gl_FragColor.xyz = (texture2D(color_texture, texture_coordinate - dir).xyz + texture2D(color_texture, texture_coordinate + dir).xyz);
  gl_FragColor.xyz += ((dir.x >= 0.0) == (dir.y >= 0.0)) ? colorNW + colorSE : colorNE + colorSW;
  gl_FragColor.xyz *= 0.25;
}
