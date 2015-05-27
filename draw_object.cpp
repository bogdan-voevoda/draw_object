#include "draw_object.h"

draw_object::draw_object() {

}

void draw_object::cleanUp() {
    vertices.clear();
    normals.clear();
    texCoord.clear();

    uint border = materials.size();
    for (uint i = 0; i < border; ++ i) {
        if (materials[i].bHasTexture) {
            glBindTexture(GL_TEXTURE_2D, materials[i].texture);
            glDeleteTextures(1, &materials[i].texture);
        }
    }

    materials.clear();
    indices.clear();

    qDebug() << "draw_object deleted";
}

draw_object::~draw_object() {
    cleanUp();
}
