#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "render_object.h"
#include "mainwindow.h"
#include "camera.h"
#include <QGLWidget>
#include <QVector>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QTouchEvent>
#include <QTime>

#define CAMERA_SPEED 200.0

class QGLShaderProgram;
class MainWindow;
class QTCamera;
class Camera;
class render_object;

class MainWidget: public QGLWidget, protected QGLFunctions {

    Q_OBJECT

public:
    explicit MainWidget(MainWindow *parent = 0);
    virtual ~MainWidget();

    // Visual modifiers
    bool isAlphaBlendingOn = false;
    bool isCullingOn       = true;
    bool lightCase         = true;
    bool isWireMode        = false;

    // Getters
    float getFPS()               { return this->FPS; }
    Camera * getCamera()         { return this->mainCamera; }
    QVector2D & mousePos()       { return this->rotPos; }
    MainWindow * getParent()     { return this->parent; }
    QMatrix4x4 & projectionMat() { return this->projection; }
protected:

    // OGL
    void resizeGL(int nWidth, int nHeight);
    void initializeGL();
    void paintGL();

    // Event handlers
    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *);
    bool event(QEvent *event);

    // Helpers
    void calculateFrameRate();
    void renderScene();
    void cameraLoop();
    void cameraUpdate();
private:
    MainWindow *parent;

    // Positional data
    QMatrix4x4  projection;
    QVector2D   startPos, rotPos;

    // Camera data
    Camera *mainCamera;

    QTime   current_time;
    QTime   last_time;
    int     elapsed_time;
    int     timerId;
    float   FPS;

    // TEMPORARY SOLUTIONS
public:
    render_object* obj;
    void delObj() { /*delete obj->draw_objects.at(0) ; qDebug() << "DELETED";*/ }
    void createObj();

    bool is_pending_camera_loop = true;
};

#endif // MAINWIDGET_H
