#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform vec4 u_light_ambient;
uniform vec4 u_light_diffuse;
uniform vec4 u_light_specular;
uniform vec3 u_light_position;

uniform sampler2D u_material_texture;
uniform vec4      u_material_ambient;
uniform vec4      u_material_diffuse;
uniform vec4      u_material_specular;
uniform vec4      u_material_emission;
uniform float     u_material_shininess;

varying vec2  v_vert_texcoord;
varying vec3  v_vert_normal;
varying vec3  v_vert_lightDir;
varying vec3  v_vert_viewDir;


void main() {
    vec3 normal   = normalize(v_vert_normal);
    vec3 lightDir = normalize(v_vert_lightDir);
    vec3 viewDir  = normalize(v_vert_viewDir);

    vec4 ambient = u_material_ambient * u_light_ambient;
    vec4 diff = u_material_diffuse * u_light_diffuse * max(dot(normal, lightDir), 0.0);
    vec3 reflection = reflect(-viewDir, normal);
    float RdotVpow = pow(max (dot (lightDir, reflection), 0.0), u_material_shininess);
    vec4 spec = u_material_specular * u_light_specular * RdotVpow;

    gl_FragColor = (u_material_emission + ambient + diff + spec) * texture2D(u_material_texture, v_vert_texcoord);
}

