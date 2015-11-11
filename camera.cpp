#include "camera.h"

Camera::Camera() {}

Camera::~Camera() {}

void Camera::setCreate(float x, float y, float z) {
    position   = QVector3D(x, y, z);
}

void Camera::cameraRotate(float x, float y, float z) {
    rotation += QVector3D(x, y, z);

//    updateProjection();
}

void Camera::cameraMove(float x, float y, float z) {
    QMatrix4x4 tmp_mat = GLRotation(rotation).transposed();
    QVector3D move = QVector3D(tmp_mat * QVector4D(x, y, z, 1.0f));

    position += move;

//    updateProjection();
}

const QMatrix4x4 Camera::GLRotation(float x, float y, float z) {
    const float cx = cosf(x * math_radians), sx = sinf(x * math_radians),
                cy = cosf(y * math_radians), sy = sinf(y * math_radians),
                cz = cosf(z * math_radians), sz = sinf(z * math_radians);

    return QMatrix4x4(  cy * cz, -cy * sz, sy, 0,
                        cx * sz + sx * cz * sy, cx * cz - sx * sy * sz, -cy * sx, 0,
                        sx * sz - cx * cz * sy, cz * sx + cx * sy * sz, cx * cy, 0,
                        0, 0, 0, 1);
}

const QMatrix4x4 Camera::GLRotation(const QVector3D &vec) {
    return GLRotation(vec.x(), vec.y(), vec.z());
}

void Camera::updateProjection() {
    QMatrix4x4 view;
    view *= GLRotation(rotation);
    view.transposed();
    view.translate(-position);

    viewProjection = projection * view;
}

//===============================================================================================================================

void Camera::getFrameTime() {
    static float frameTime = 0.0f;
    QTime time = QTime::currentTime();
    float currentTime = time.msec() * 0.001f;

    if (currentTime >= frameTime) {
        g_FrameInterval = currentTime - frameTime;
    }
    frameTime = currentTime;
}

//==============================================================================================================================

void Camera::checkCameraCollision(QVector3D *pVertices, int numOfVerts) {
    for( int i = 0; i < numOfVerts; i += 3 ) {
        QVector3D vTriangle[3] = { pVertices[i], pVertices[i+1], pVertices[i+2] };

        QVector3D vNormal = QVector3D::normal(vTriangle[0], vTriangle[1], vTriangle[2]);

        float distance = 0.0f;

        int classification = classifySphere(position, vNormal, vTriangle[0], m_radius, distance);

        if ( classification == INTERSECTS ) {

            QVector3D vOffset = vNormal * distance;

            QVector3D vIntersection = position - vOffset;

            if (insidePolygon(vIntersection, vTriangle, 3) || edgeSphereCollision(position, vTriangle, 3, m_radius / 2)) {

                vOffset = getCollisionOffset(vNormal, m_radius, distance);

                position += vOffset;
            } /*else {
                QVector3D pseudo_gravity = QVector3D(0.0, -0.0005, 0.0);

                position += pseudo_gravity;
            }*/
        }
    }
}

bool Camera::edgeSphereCollision(QVector3D &vCenter, QVector3D vPolygon[], int vertexCount, float radius) {
    QVector3D vPoint;

    for( int i = 0; i < vertexCount; i++ ) {
        vPoint = closestPointOnLine(vPolygon[i], vPolygon[(i + 1) % vertexCount], vCenter);
        float distance = vCenter.distanceToPoint(vPoint);

        if(distance < radius) {
            return true;\
        }
    }
    return false;
}

QVector3D Camera::getCollisionOffset(QVector3D &vNormal, float radius, float distance) {
    QVector3D vOffset = QVector3D(0, 0, 0);

    if(distance > 0) {
        float distanceOver = radius - distance;
        vOffset = vNormal * distanceOver;
    } else {
        float distanceOver = radius + distance;
        vOffset = vNormal * -distanceOver;
    }

    return vOffset;
}

int Camera::classifySphere(QVector3D &vCenter, QVector3D &vNormal, QVector3D &vPoint, float radius, float &distance) {

    float d = (float)planeDistance(vNormal, vPoint);

    distance = (vNormal.x() * vCenter.x() + vNormal.y() * vCenter.y() + vNormal.z() * vCenter.z() + d);

    if(std::abs(distance) < radius) {
        return INTERSECTS;
    } else if (distance >= radius) {
        return FRONT;
    }
    return BEHIND;
}

float Camera::planeDistance(QVector3D Normal, QVector3D Point) {
    float distance = 0;

    distance = - ((Normal.x() * Point.x()) + (Normal.y() * Point.y()) + (Normal.z() * Point.z()));

    return distance;
}

bool Camera::insidePolygon(QVector3D vIntersection, QVector3D Poly[], long verticeCount) {
    const double MATCH_FACTOR = 0.99;
    double Angle = 0.0;
    QVector3D vA, vB;

    for (int i = 0; i < verticeCount; i++) {
        vA = Poly[i] - vIntersection;

        vB = Poly[(i + 1) % verticeCount] - vIntersection;

        Angle += angleBetweenVectors(vA, vB);
    }

    if( Angle >= ( MATCH_FACTOR * (2.0 * math_pi) ) ) {
        return true;
    }
    return false;
}

double Camera::angleBetweenVectors(QVector3D Vector1, QVector3D Vector2) {

    float dotProduct = QVector3D::dotProduct(Vector1, Vector2);
    float vectorsMagnitude = magnitude(Vector1) * magnitude(Vector2);
    double angle = std::acos( dotProduct / vectorsMagnitude );

    if(std::isnan(angle)) {
        return 0;
    }
    return(angle);
}

float Camera::magnitude(QVector3D vNormal) {
    return (float)sqrt( (vNormal.x() * vNormal.x()) + (vNormal.y() * vNormal.y()) + (vNormal.z() * vNormal.z()) );
}

QVector3D Camera::closestPointOnLine(QVector3D vA, QVector3D vB, QVector3D vPoint) {
    QVector3D vVector1 = vPoint - vA;
    QVector3D vVector2 = QVector3D(vB - vA);
    vVector2.normalize();

    float d = vA.distanceToPoint(vB);
    float t = QVector3D::dotProduct(vVector2, vVector1);

    if (t <= 0) {
        return vA;
    }

    if (t >= d) {
        return vB;
    }

    QVector3D vVector3 = vVector2 * t;
    QVector3D vClosestPoint = vA + vVector3;

    return vClosestPoint;
}

//===============================================================================================================================

void Camera::normalizePlane(float frustum[6][4], int side) {
    float magnitude = (float)sqrt( frustum[side][A] * frustum[side][A] +
                                   frustum[side][B] * frustum[side][B] +
                                   frustum[side][C] * frustum[side][C] );

    frustum[side][A] /= magnitude;
    frustum[side][B] /= magnitude;
    frustum[side][C] /= magnitude;
    frustum[side][D] /= magnitude;
}

void Camera::calculateFrustum(QMatrix4x4 &projection, QMatrix4x4 &model) {
//    float   proj[16];
//    float   modl[16];
    float   clip[16];

    const float *proj = projection.constData();
    const float *modl = model.constData();

    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

    m_Frustum[RIGHT][A] = clip[ 3] - clip[ 0];
    m_Frustum[RIGHT][B] = clip[ 7] - clip[ 4];
    m_Frustum[RIGHT][C] = clip[11] - clip[ 8];
    m_Frustum[RIGHT][D] = clip[15] - clip[12];

    normalizePlane(m_Frustum, RIGHT);

    m_Frustum[LEFT][A] = clip[ 3] + clip[ 0];
    m_Frustum[LEFT][B] = clip[ 7] + clip[ 4];
    m_Frustum[LEFT][C] = clip[11] + clip[ 8];
    m_Frustum[LEFT][D] = clip[15] + clip[12];

    normalizePlane(m_Frustum, LEFT);

    m_Frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
    m_Frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
    m_Frustum[BOTTOM][C] = clip[11] + clip[ 9];
    m_Frustum[BOTTOM][D] = clip[15] + clip[13];

    normalizePlane(m_Frustum, BOTTOM);

    m_Frustum[TOP][A] = clip[ 3] - clip[ 1];
    m_Frustum[TOP][B] = clip[ 7] - clip[ 5];
    m_Frustum[TOP][C] = clip[11] - clip[ 9];
    m_Frustum[TOP][D] = clip[15] - clip[13];

    normalizePlane(m_Frustum, TOP);

    m_Frustum[BACK][A] = clip[ 3] - clip[ 2];
    m_Frustum[BACK][B] = clip[ 7] - clip[ 6];
    m_Frustum[BACK][C] = clip[11] - clip[10];
    m_Frustum[BACK][D] = clip[15] - clip[14];

    normalizePlane(m_Frustum, BACK);

    m_Frustum[FRONT][A] = clip[ 3] + clip[ 2];
    m_Frustum[FRONT][B] = clip[ 7] + clip[ 6];
    m_Frustum[FRONT][C] = clip[11] + clip[10];
    m_Frustum[FRONT][D] = clip[15] + clip[14];

    normalizePlane(m_Frustum, FRONT);
}

bool Camera::pointInFrustum(float x, float y, float z) {
    for(int i = 0; i < 6; i++ ) {
        if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0) {
            return false;
        }
    }

    return true;
}

bool Camera::sphereInFrustum(float x, float y, float z, float radius) {
    for(int i = 0; i < 6; i++ ) {
        if ( m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius ) {
            return false;
        }
    }

    return true;
}

bool Camera::cubeInFrustum(float x, float y, float z, float size) {
    for(int i = 0; i < 6; i++ ) {
        if (  (m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] <= -size)
           && (m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] <= -size)
           && (m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] <= -size)
           && (m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] <= -size)
           && (m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] <= -size)
           && (m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] <= -size)
           && (m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] <= -size)
           && (m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] <= -size)) {
            return false;
        }
    }

    return true;
}

