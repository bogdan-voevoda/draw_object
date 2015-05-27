#ifdef GL_ES
precision mediump int;
precision highp float;
#endif

//#version 100
//#extension GL_EXT_gpu_shader4 : enable

uniform vec4 u_light_ambient;
uniform vec4 u_light_diffuse;
uniform vec4 u_light_specular;
uniform vec3 u_light_attenuation;

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

//--------------------------------------
//uniform sampler2D shadowMap;
//uniform int u_sm_width;
//uniform int u_sm_height;
//varying vec4 lpos;

//vec2 poissonDiskSelector(int i) {
//    if ( i == 0)
//        return vec2 ( -0.94201624,  -0.39906216 );
//    if ( i == 1)
//        return vec2 (  0.94558609,  -0.76890725 );
//    if ( i == 2)
//        return vec2 ( -0.094184101, -0.92938870 );
//    if ( i == 3)
//        return vec2 (  0.34495938,   0.29387760 );
//    if ( i == 4)
//        return vec2 (  0.34495938,   0.29387760 );
//    if ( i == 5)
//        return vec2 ( -0.81544232,  -0.87912464 );
//    if ( i == 6)
//        return vec2 ( -0.38277543,   0.27676845 );
//    if ( i == 7)
//        return vec2 (  0.97484398,   0.75648379 );
//    if ( i == 8)
//        return vec2 (  0.44323325,  -0.97511554 );
//    if ( i == 9)
//        return vec2 (  0.53742981,  -0.47373420 );
//    if ( i == 10)
//        return vec2 ( -0.26496911,  -0.41893023 );
//    if ( i == 11)
//        return vec2 (  0.79197514,   0.19090188 );
//    if ( i == 12)
//        return vec2 ( -0.24188840,   0.99706507 );
//    if ( i == 13)
//        return vec2 ( -0.81409955,   0.91437590 );
//    if ( i == 14)
//        return vec2 (  0.19984126,   0.78641367 );
//    else
//        return vec2 (  0.14383161,  -0.14100790 );
//}

//--------------------------------------

//float calcBias() {
//    float bias;
//    vec3 n = normalize( v_vert_normal );
//    vec3 l = normalize(vec3(lpos.xyz));
//    float cosTheta = clamp( dot( n,l ), 0.0, 1.0 );

//    bias = 0.0001*tan(acos(cosTheta));
//    bias = clamp(bias, 0.0, 0.01);

//    return bias;
//}

//float randomizer(vec4 i) {
//    float dot_product = dot(i, vec4(12.9898,78.233,45.164,94.673));
//    return fract(sin(dot_product) * 43758.5453);
//}

//bool isBorderPoint(vec3 shadow_coord) {
//    float width = 1/u_sm_width;
//    float height = 1/u_sm_height;

//    return
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x - width, shadow_coord.y + height)).x) &&
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x, shadow_coord.y + height)).x) &&
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x + width, shadow_coord.y + height)).x) &&
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x - width, shadow_coord.y)).x) &&
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x, shadow_coord.y)).x) &&
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x + width, shadow_coord.y)).x) &&
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x - width, shadow_coord.y - height)).x) &&
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x, shadow_coord.y - height)).x) &&
//    (shadow_coord.z <= texture2D(shadowMap, vec2(shadow_coord.x + width, shadow_coord.y - height)).x);
//}

void main() {
    //--------------------------------------

//    float shadow = 0.0;

//    if (lpos.w > 0.0) {
//        vec3 smcoord = lpos.xyz / lpos.w;

//        for (int i = 0; i < 4 ; i += 1) {

//            int index = int(16.0 * randomizer(vec4(gl_FragCoord.xyz, i)))%16;

//            shadow += float(smcoord.z - 0.007 <= texture2D(shadowMap, smcoord.xy +
//                            poissonDiskSelector(index)/800.0).x);
//        }
//        shadow /= 4.0;

        //--------------------------------------
//        float shadow = float(smcoord.z - 0.007 <= texture2D(shadowMap, smcoord.xy).x);
        //--------------------------------------
//    }

    float attenuation = 1.0 / (u_light_attenuation.x +
            u_light_attenuation.y * v_vert_distance +
            u_light_attenuation.z * v_vert_distance * v_vert_distance);

    vec3 normal   = normalize(v_vert_normal);
    vec3 lightDir = normalize(v_vert_lightDir);
    vec3 viewDir  = normalize(v_vert_viewDir);

    vec4 color = u_material_emission;
    color += u_material_ambient * u_light_ambient * attenuation;
    float NdotL = max(dot(normal, lightDir), 0.0);
    color += u_material_diffuse * u_light_diffuse * NdotL * attenuation;
    float RdotVpow = max(pow(dot(reflect(-lightDir, normal), viewDir), u_material_shininess), 0.0);
    color += u_material_specular * u_light_specular * RdotVpow * attenuation;
    color *= texture2D(u_material_texture, v_vert_texcoord);

    //--------------------------------------
//    color = mix(color*shadow, color, 0.5);
    //--------------------------------------

    gl_FragColor = color;
}




