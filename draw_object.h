#ifndef DRAW_OBJECT_H
#define DRAW_OBJECT_H

#include <QGL>
#include <vector>
#include <QVector3D>
#include <QVector4D>
#include <QVector2D>
#include <QMatrix4x4>
#include <QGLShaderProgram>

struct VertexData {
    QVector3D position;
    QVector3D normCoord;
    QVector2D texCoord;
};

struct Material {
    GLuint    texture;
    QVector4D rgba;

    QVector4D ambient;
    QVector4D diffuse;
    QVector4D specular;
    QVector4D emission;
    float     shininess;

    bool bHasTexture = false;
};

struct draw_model {
    draw_model():numOfDrawObjects(0) {}

    uint numOfDrawObjects;

    uint *drawObjectsIndex;
};


class draw_object {
public:
    draw_object();
    ~draw_object();
    void cleanUp();

    // POSITIONAL DATA

    QMatrix4x4 model;

    // VERTICES DATA

    std::vector<QVector3D>   vertices;
    std::vector<QVector3D>   normals;
    std::vector<QVector2D>   texCoord;
    std::vector<Material>    materials;
    std::vector<GLuint>      indices;

    // SHADER PROGRAM
    QGLShaderProgram *shaderProgram = 0;

    // TRIANGULIATION MODE
    GLenum mode = GL_TRIANGLES;

    // VBO DATA
    uint positionInVBO;

    uint index_shift = 0;
};

#endif // DRAW_OBJECT_H
