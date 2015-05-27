#ifdef GL_ES
precision lowp float;
#endif

//#version 100

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

uniform mat4 u_transform_model;
uniform mat4 u_transform_viewProjection;
uniform mat3 u_transform_normal;

varying vec2  v_vert_texcoord;
varying vec3  v_vert_normal;


void main() {
    vec4 vertex   = u_transform_model * vec4(a_position, 1.0);

    v_vert_texcoord = a_texcoord;
    v_vert_normal   = u_transform_normal * a_normal;

    gl_Position = u_transform_viewProjection * vertex;
}
