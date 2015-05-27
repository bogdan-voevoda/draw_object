#ifndef TERRAIN_H
#define TERRAIN_H

#include "draw_object.h"
#include <QVector2D>
#include <QVector3D>


#define STEP_SIZE		16

class terrain : public draw_object {
public:
    terrain(float x, float y, float z, GLuint *terrainTextures, uint ind_shift);
    ~terrain();
    void loadRawFile(QString &strName, std::vector<QVector3D> *vertices,
                                            std::vector<QVector3D> *normals,
                                                std::vector<QVector2D> *texCoord,
                                                    std::vector<GLuint> *indices);
    void draw();
private:
    uint mapSize = 0;
    QVector2D setTextureCoord(float x, float z);
    int height(QByteArray &pHeightMap, int X, int Y);
};

#endif // TERRAIN_H
