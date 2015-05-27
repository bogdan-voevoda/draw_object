#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

uniform mat4 u_transform_model;
uniform mat4 u_transform_viewProjection;
uniform mat4 u_transform_normal;
uniform vec3 u_transform_viewPosition;

uniform vec4 u_light_position;
uniform vec4 u_light_ambient;
uniform vec4 u_light_diffuse;
uniform vec4 u_light_specular;

varying vec2  v_vert_texcoord;
varying vec3  v_vert_normal;
varying vec3  v_vert_lightDir;
varying vec3  v_vert_viewDir;

void main() {
    vec4 vertex   = u_transform_model * vec4(a_position, 1.0);

    v_vert_texcoord = a_texcoord;
    v_vert_normal   = vec3(u_transform_normal * vec4(a_normal, 1.0));
    v_vert_lightDir = vec3(u_light_position);
    v_vert_viewDir  = u_transform_viewPosition - vec3(vertex);

    gl_Position = u_transform_viewProjection * vertex;
}
