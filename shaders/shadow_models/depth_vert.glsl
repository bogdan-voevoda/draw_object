attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

uniform mat4 u_transform_viewProjection;
uniform mat4 u_transform_model;

void main() {
    vec4 vertex = u_transform_model * vec4(a_position, 1.0);

    gl_Position = u_transform_viewProjection * vertex;
}
