#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform vec4 u_light_ambient;
uniform vec4 u_light_diffuse;
uniform vec4 u_light_specular;
uniform vec3 u_light_attenuation;
uniform vec3  u_light_spotDirection;
uniform float u_light_spotExponent;
uniform float u_light_spotCosCutoff;

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
varying float v_vert_distance;

void main() {
    float spotEffect = dot(normalize(u_light_spotDirection), -v_vert_lightDir);
    float spot       = float(spotEffect > u_light_spotCosCutoff);
    spotEffect = max(pow(spotEffect, u_light_spotExponent), 0.0);

    float attenuation = spot * spotEffect / (u_light_attenuation.x +
            u_light_attenuation.y * v_vert_distance +
            u_light_attenuation.z * v_vert_distance * v_vert_distance);

    vec3 normal   = normalize(v_vert_normal);
    vec3 lightDir = normalize(v_vert_lightDir);
    vec3 viewDir  = normalize(v_vert_viewDir);

    vec4 ambient = u_material_ambient * u_light_ambient * attenuation;
    vec4 diff = u_material_diffuse * u_light_diffuse * max(dot (normal, lightDir), 0.0) * attenuation;
    vec3 reflection = reflect(-viewDir, normal);
    float RdotVpow = pow(max(dot(lightDir, reflection), 0.0), u_material_shininess);
    vec4 spec = u_material_specular * u_light_specular * RdotVpow * attenuation;
    gl_FragColor = (u_material_emission + ambient + diff + spec) * texture2D(u_material_texture, v_vert_texcoord);
}

