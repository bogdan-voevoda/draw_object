#include "skybox.h"

skybox::skybox(float x, float y, float z, float width, float height, float length, GLuint *skyBoxTextures, uint ind_shift){
    index_shift = ind_shift;

    QMatrix4x4 model;
    model.translate(x, y, z);
    this->model = model;

    x = x - width  / 2;
    y = y - height / 2;
    z = z + length / 2;

    //  VERTICES
    //  [BOTTOM];
    vertices.push_back(QVector3D(x,       y,        z       ));
    vertices.push_back(QVector3D(x,       y,        z-length));
    vertices.push_back(QVector3D(x+width, y,        z-length));
    vertices.push_back(QVector3D(x+width, y,        z       ));
    //  [TOP];
    vertices.push_back(QVector3D(x,       y+height, z       ));
    vertices.push_back(QVector3D(x+width, y+height, z       ));
    vertices.push_back(QVector3D(x+width, y+height, z-length));
    vertices.push_back(QVector3D(x,       y+height, z-length));
    //  [FRONT];
    vertices.push_back(QVector3D(x,       y,        z       ));
    vertices.push_back(QVector3D(x+width, y,        z       ));
    vertices.push_back(QVector3D(x+width, y+height, z       ));
    vertices.push_back(QVector3D(x,       y+height, z       ));
    //  [BACK];
    vertices.push_back(QVector3D(x+width, y,        z-length));
    vertices.push_back(QVector3D(x,       y,        z-length));
    vertices.push_back(QVector3D(x,       y+height, z-length));
    vertices.push_back(QVector3D(x+width, y+height, z-length));
    //  [LEFT];
    vertices.push_back(QVector3D(x+width, y,        z       ));
    vertices.push_back(QVector3D(x+width, y,        z-length));
    vertices.push_back(QVector3D(x+width, y+height, z-length));
    vertices.push_back(QVector3D(x+width, y+height, z       ));
    //  [RIGHT];
    vertices.push_back(QVector3D(x,       y,        z-length));
    vertices.push_back(QVector3D(x,       y,        z       ));
    vertices.push_back(QVector3D(x,       y+height, z       ));
    vertices.push_back(QVector3D(x,       y+height, z-length));

    //  TEX COORDS
    for (uint i = 0; i < 6; ++i) {
        texCoord.push_back(QVector2D(0.0f, 0.0f));
        texCoord.push_back(QVector2D(1.0f, 0.0f));
        texCoord.push_back(QVector2D(1.0f, 1.0f));
        texCoord.push_back(QVector2D(0.0f, 1.0f));
    }

    //  NORMALS
    for (uint i = 0; i < 24; ++i) {
        normals.push_back(QVector3D(0.0f, 0.0f, 0.0f));
    }

    //  INDICES
    uint border = ind_shift + 36;
    for (uint shift = ind_shift; shift < border; shift += 4) {
        indices.push_back(shift + 1);
        indices.push_back(shift + 0);
        indices.push_back(shift + 3);
        indices.push_back(shift + 3);
        indices.push_back(shift + 2);
        indices.push_back(shift + 1);
    }

    //  MATERIALS
    for (uint i = 0; i < 6; ++i) {
        Material material;
        material.ambient = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
        material.diffuse = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
        material.emission = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
        material.shininess = 1.0f;
        material.specular = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
        material.texture = skyBoxTextures[i];
        material.bHasTexture = true;

        materials.push_back(material);
    }
}


skybox::~skybox() {

}

void skybox::draw() {
    if ( shaderProgram ) {
        QMatrix4x4 normal = model.inverted();

        shaderProgram->setUniformValue("u_transform_model", model);
        shaderProgram->setUniformValue("u_transform_normal", normal.normalMatrix());

        uint vboPos = positionInVBO;

        for (uint i = 0; i < 6; ++i) {
            glBindTexture(GL_TEXTURE_2D, materials.at(i).texture);
            shaderProgram->setUniformValue("u_material_texture", 0);

            glDrawElements(mode, 6, GL_UNSIGNED_INT, (void*)(vboPos));
            vboPos += 6 * sizeof(GLuint);
        }
    }
}

