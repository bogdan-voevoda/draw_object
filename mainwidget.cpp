#include "mainwidget.h"

MainWidget::MainWidget(MainWindow *parent): parent(parent) {
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    grabGesture(Qt::PinchGesture);

    mainCamera = new Camera();

    mainCamera->setCreate(300, 300, 300);

    timerId = startTimer(0);
}

MainWidget::~MainWidget() {
    killTimer(timerId);
    delete mainCamera;
    if (obj) { delete obj; }
}

// OGL

void MainWidget::initializeGL() {
    initializeGLFunctions();

//    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    qglClearColor(Qt::gray);

    obj = new render_object(this);
    obj->createObject();
}

void MainWidget::createObj() {
    if (!obj) {
        obj = new render_object(this);
        obj->createObject();
    }
}

void MainWidget::resizeGL(int nWidth, int nHeight) {
    glViewport(0, 0, nWidth, nHeight);
    qreal aspect = qreal(nWidth) / qreal(nHeight ? nHeight : 1);
    const qreal zNear = 1, zFar = 10000.0, fov = 30.0;
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);

    mainCamera->projection = this->projection;
}

void MainWidget::paintGL() {
    if (isAlphaBlendingOn) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }

    if (isCullingOn) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

#ifndef Q_OS_ANDROID
    if (isWireMode) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    calculateFrameRate();

    renderScene();
}

// Event handlers

void MainWidget::mouseMoveEvent(QMouseEvent *e) {
    rotPos = QVector2D(e->localPos());
}

void MainWidget::mouseDoubleClickEvent(QMouseEvent *) {
    is_pending_camera_loop = !is_pending_camera_loop;
}

void MainWidget::timerEvent(QTimerEvent *) {
    if (is_pending_camera_loop) {
        cameraLoop();
    }
}


bool MainWidget::event(QEvent *event) {
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
        if (touchPoints.count() == 2) {
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();

            float nSca = 1.0;

            qreal currentScaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                    / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                nSca *= currentScaleFactor;
                currentScaleFactor = 1.0;
            }
            nSca *= currentScaleFactor;

            if (nSca > 1.0) {
                parent->isMoveUp = true;
            } else if (nSca < 1.0) {
                parent->isMoveDown = true;
            }
        } else if (touchPoints.count() == 1) {
            const QTouchEvent::TouchPoint &touchPoint = touchPoints.first();

            startPos = QVector2D(touchPoint.startPos());
            rotPos = QVector2D(touchPoint.lastPos());

            parent->isMoveUp = parent->isMoveDown = false;
        } else {
            parent->isMoveUp = parent->isMoveDown = false;
        }
        return true;
    }
    default:
        break;
    }

    return QWidget::event(event);
}

// Helpers

void MainWidget::cameraLoop() {
    float speed = CAMERA_SPEED * mainCamera->g_FrameInterval;

    if (parent->isMoveUp) {
        mainCamera->cameraMove(0.0f, 0.0f, -speed);
    }

    if (parent->isMoveDown) {
        mainCamera->cameraMove(0.0f, 0.0f, speed);
    }
    if (parent->isMoveLeft) {
        mainCamera->cameraMove(-speed, 0.0f, 0.0f);
    }
    if (parent->isMoveRight) {
        mainCamera->cameraMove(speed, 0.0f, 0.0f);
    }

    if (parent->isMoveUp || parent->isMoveDown || parent->isMoveLeft || parent->isMoveRight) {
        mainCamera->isMoving = true;
    } else {
        mainCamera->isMoving = false;
    }

    cameraUpdate();

    mainCamera->getFrameTime();

    updateGL();
}

void MainWidget::cameraUpdate() {

#ifdef Q_OS_ANDROID

    if( (startPos == rotPos) ) return;

    float speed = 0.01;
    static float rotateDelta[2];

    rotateDelta[0] = rotateDelta[1] = 0.0f;

    rotateDelta[0] += (rotPos.x() - startPos.x());
    rotateDelta[1] += (rotPos.y() - startPos.y());

    mainCamera->cameraRotate(speed * rotateDelta[1], speed * rotateDelta[0], 0.0f);

    rotPos = startPos = QVector2D(0.0, 0.0);

#else

    QPoint mousePos(parent->cursor().pos());
    QPoint widgetCenter = parent->geometry().center();
    float middleX = widgetCenter.x();
    float middleY = widgetCenter.y();

    parent->cursor().setPos(middleX, middleY);

    if( (mousePos.x() == middleX) && (mousePos.y() == middleY) ) return;

    float speed = 0.1;
    static float rotateDelta[2];

    rotateDelta[0] = rotateDelta[1] = 0.0f;

    rotateDelta[0] += mousePos.x() - middleX;
    rotateDelta[1] += mousePos.y() - middleY;

    mainCamera->cameraRotate(speed * rotateDelta[1], speed * rotateDelta[0], 0.0f);
#endif
}

void MainWidget::renderScene() {
    if (obj && obj->isVisiblse) {
        obj->renderObject();
    }
}

void MainWidget::calculateFrameRate() {
    last_time = current_time;
    current_time = QTime::currentTime();
    elapsed_time = (current_time.second()*1000 + current_time.msec()) -
                      (last_time.second()*1000 + last_time.msec());

    if (elapsed_time) {
        this->FPS = (float)1000.0 / (float)elapsed_time;
    }
}
