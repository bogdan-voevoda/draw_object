#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QGL>
#include <vector>
#include <fstream>
#include <sstream>


#include "draw_object.h"
#include "mainwidget.h"
#include "camera.h"
#include "skybox/skybox.h"
#include "terrain/terrain.h"


class Camera;
class MainWidget;
class QGLFunctions;
class QGLShaderProgram;
class draw_object;
struct draw_model;
struct Material;

struct PointLight {
    QVector4D  position;
    QVector4D  ambient;
    QVector4D  diffuse;
    QVector4D  specular;
    QVector3D  attenuation;
};

class render_object : public QObject,  protected QGLFunctions{

    Q_OBJECT

private:
public: // PUBLIC FOR TESTING
    MainWidget *parent;

    std::vector<draw_object *>      draw_objects;
    std::vector<draw_model *>       draw_models;
    std::vector<QGLShaderProgram *> shader_programs;

    GLuint                          vbo[2];
    uint                            vboVerticesDataSize = 0;
    uint                            vboIndicesDataSize  = 0;

public:
    render_object(MainWidget *parent = 0);

    virtual ~render_object();

    void initShaders(const QString &vertSaderLocation, const QString &fragSaderLocation, QGLShaderProgram &program);

    GLuint initTexture(const QString &textureLocation);

    void initGeometry(GLuint *vbo);

    void bindGeometry(QGLShaderProgram *program);

    void drawObjectWithProgram(QGLShaderProgram *program, draw_object *obj);

    void drawObject(draw_object *obj);

    void drawModel(draw_model *model);

    void createObject();

    void renderObject();

    void pointLightSetup(QGLShaderProgram &program, const PointLight &light);
    void materialSetup(QGLShaderProgram &program, const Material &material);
    void cameraSetup(QGLShaderProgram *program);

    //==========================================================================================

    PointLight light;

    bool       isVisiblse;

    void addVertices(std::vector<QVector3D> *vertices,
                     std::vector<QVector3D> *normals,
                     std::vector<QVector2D> *texCoord,
                     std::vector<GLuint> *indices,
                     uint &indexer, uint *sides,
                     float x, float y, float z, float size);

    void indexSeter(std::vector<GLuint> *indices, uint &indexer);

    //==============================================================================================================
    bool isPending = true;

    void objectRotation(QVector3D &objPos, float dt);
};


#endif // RENDER_OBJECT_H
