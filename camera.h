// tst

#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QQuaternion>
#include "mainwidget.h"

enum FrustumSide {
    RIGHT	= 0,		// The RIGHT side of the frustum
    LEFT	= 1,		// The LEFT	 side of the frustum
    BOTTOM	= 2,		// The BOTTOM side of the frustum
    TOP		= 3,		// The TOP side of the frustum
    BACK	= 4,		// The BACK	side of the frustum
    FRONT	= 5			// The FRONT side of the frustum
};

enum PlaneData {
    A = 0,				// The X value of the plane's normal
    B = 1,				// The Y value of the plane's normal
    C = 2,				// The Z value of the plane's normal
    D = 3				// The distance the plane is from the origin
};

#define BEHIND		0
#define INTERSECTS	1
#define FRONT		2

class MainWidget;

const float math_pi      = 3.1415926536f;
const float math_radians = math_pi / 180.0f;

class Camera : public QObject {
    Q_OBJECT
public:
    QVector3D position = QVector3D(0,0,0);
    QVector3D rotation;
    QMatrix4x4 projection;
    QMatrix4x4 viewProjection;

    float g_FrameInterval = 1.0f;
    bool isMoving = false;

    Camera();
    ~Camera();

    void setCreate(float x, float y, float z);
    void cameraRotate(float x, float y, float z);
    void cameraMove(float x, float y, float z);
    void updateProjection();

    const QMatrix4x4 GLRotation(float x, float y, float z);
    const QMatrix4x4 GLRotation(const QVector3D &vec);

    void getFrameTime();

    // collison
    float m_radius = 10.0f;
    void checkCameraCollision(QVector3D *pVertices, int numOfVerts);
    void setCameraRadius(float radius) { m_radius = radius; }
    bool edgeSphereCollision(QVector3D &vCenter, QVector3D vPolygon[], int vertexCount, float radius);
    int classifySphere(QVector3D &vCenter, QVector3D &vNormal, QVector3D &vPoint, float radius, float &distance);
    float planeDistance(QVector3D Normal, QVector3D Point);
    bool insidePolygon(QVector3D vIntersection, QVector3D Poly[], long verticeCount);
    double angleBetweenVectors(QVector3D Vector1, QVector3D Vector2);
    float magnitude(QVector3D vNormal);
    QVector3D closestPointOnLine(QVector3D vA, QVector3D vB, QVector3D vPoint);
    QVector3D getCollisionOffset(QVector3D &vNormal, float radius, float distance);

    //frustrum culling
    float m_Frustum[6][4];

    void normalizePlane(float frustum[6][4], int side);
    void calculateFrustum(QMatrix4x4 &projection, QMatrix4x4 &model);
    bool pointInFrustum(float x, float y, float z);
    bool sphereInFrustum(float x, float y, float z, float radius);
    bool cubeInFrustum(float x, float y, float z, float size);
};

#endif // CAMERA_H
