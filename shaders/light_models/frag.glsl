#ifdef GL_ES
precision lowp float;
#endif

//#version 100
//#extension GL_EXT_gpu_shader4 : enable


uniform sampler2D u_material_texture;

varying vec2  v_vert_texcoord;

void main() {
    gl_FragColor = texture2D(u_material_texture, v_vert_texcoord);
}




