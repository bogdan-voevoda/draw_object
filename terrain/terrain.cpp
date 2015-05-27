#include "terrain.h"
#include <QFile>
#include <QGLShaderProgram>

terrain::terrain(float x, float y, float z, GLuint *terrainTextures, uint ind_shift) {
    mode = GL_TRIANGLE_STRIP;

    index_shift = ind_shift;

    QMatrix4x4 model;
    model.translate(x, y, z);
    this->model = model;

    Material material;
    material.ambient = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
    material.diffuse = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
    material.emission = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
    material.shininess = 1.0f;
    material.specular = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
    material.texture = terrainTextures[0];
    material.bHasTexture = true;

    materials.push_back(material);
}

terrain::~terrain() {

}

void terrain::draw() {
    if ( shaderProgram ) {
        QMatrix4x4 normal = model.inverted();

        shaderProgram->setUniformValue("u_transform_model", model);
        shaderProgram->setUniformValue("u_transform_normal", normal.normalMatrix());
        glBindTexture(GL_TEXTURE_2D, materials.at(0).texture);
        shaderProgram->setUniformValue("u_material_texture", 0);

        glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, (void*)(positionInVBO));
    }
}

int terrain::height(QByteArray &pHeightMap, int X, int Y) {

    int x = X % mapSize;
    int y = Y % mapSize;

    return pHeightMap[x + (y * mapSize)];
}


QVector2D terrain::setTextureCoord(float x, float z) {
    return QVector2D((float)x / (float)mapSize, (float)z / (float)mapSize);
}


void terrain::loadRawFile(QString &strName, std::vector<QVector3D> *vertices, std::vector<QVector3D> *normals, std::vector<QVector2D> *texCoord, std::vector<GLuint> *indices) {
    QFile file(strName);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug("Can't find the height map!");
        return;
    }

    QByteArray pHeightMap(file.readAll());

    mapSize = std::sqrt(pHeightMap.size());

    file.close();

    int X = 0, Y = 0;
    int x, y, z;
    bool bSwitchSides = false;

    GLuint indexer = index_shift;

    for ( X = 0; X <= mapSize; X += STEP_SIZE ) {

        if(bSwitchSides) {

            for ( Y = mapSize; Y >= 0; Y -= STEP_SIZE ) {

                x = X;
                y = height(pHeightMap, X, Y);
                z = Y;

                vertices->push_back(QVector3D(x, y, z));
                normals->push_back(QVector3D(0,0,0));
                texCoord->push_back(setTextureCoord(x, z));
                indices->push_back(indexer);
                indexer++;

                x = X + STEP_SIZE;
                y = height(pHeightMap, X + STEP_SIZE, Y );
                z = Y;

                vertices->push_back(QVector3D(x, y, z));
                normals->push_back(QVector3D(0,0,0));
                texCoord->push_back(setTextureCoord(x, z));
                indices->push_back(indexer);
                indexer++;

            }
        } else {

            for ( Y = 0; Y <= mapSize; Y += STEP_SIZE ) {

                x = X + STEP_SIZE;
                y = height(pHeightMap, X + STEP_SIZE, Y );
                z = Y;

                vertices->push_back(QVector3D(x, y, z));
                normals->push_back(QVector3D(0,0,0));
                texCoord->push_back(setTextureCoord(x, z));
                indices->push_back(indexer);
                indexer++;

                x = X;
                y = height(pHeightMap, X, Y );
                z = Y;

                vertices->push_back(QVector3D(x, y, z));
                normals->push_back(QVector3D(0,0,0));
                texCoord->push_back(setTextureCoord(x, z));
                indices->push_back(indexer);
                indexer++;
            }
        }

        bSwitchSides = !bSwitchSides;
    }
}


