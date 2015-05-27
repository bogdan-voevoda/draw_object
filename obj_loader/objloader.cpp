#include "objloader.h"
#include <qDebug>

bool objLoader::importObj(t3DModel *pModel, char *strFileName) {
    char strMessage[255] = {0}; // This will be used for error messages

    if(!pModel || !strFileName) return false;

    m_FilePointer = fopen(strFileName, "r");

    if(!m_FilePointer) {
        qDebug("Unable to find or open the file: %s", strFileName);
        return false;
    }

    readObjFile(pModel);

    computeNormals(pModel);

    fclose(m_FilePointer);

    return true;
}

void objLoader::readObjFile(t3DModel *pModel) {
    char strLine[255] = {0};
    char ch = 0;

    while(!feof(m_FilePointer)) {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        ch = fgetc(m_FilePointer);

        switch(ch) {
            case 'v':
                if(m_bJustReadAFace) {
                    fillInObjectInfo(pModel);
                }
                readVertexInfo();
                break;
            case 'f':
                readFaceInfo();
                break;
            case '\n':
                break;
            default:
                fgets(strLine, 100, m_FilePointer);
                break;
        }
    }

    fillInObjectInfo(pModel);
}

void objLoader::readVertexInfo() {
    QVector3D vNewVertex;
    QVector2D vNewTexCoord;
    char strLine[255] = {0};
    char ch = 0;

    ch = fgetc(m_FilePointer);

    if(ch == ' ') {
        float x, y, z;
        fscanf(m_FilePointer, "%f %f %f", &x, &y, &z);
        vNewVertex = QVector3D(x, y, z);

        fgets(strLine, 100, m_FilePointer);

        m_pVertices.push_back(vNewVertex);
    } else if(ch == 't') {
        // Here we read in a texture coordinate.  The format is "vt u v"
        float u, v;
        fscanf(m_FilePointer, "%f %f", &u, &v);
        vNewTexCoord = QVector2D(u, v);

        fgets(strLine, 100, m_FilePointer);

        m_pTextureCoords.push_back(vNewTexCoord);

        m_bObjectHasUV = true;
    } else {
        fgets(strLine, 100, m_FilePointer);
    }
}

void objLoader::readFaceInfo() {
    tFace newFace     = {0};
    char strLine[255] = {0};

    if(m_bObjectHasUV ) {
        fscanf(m_FilePointer, "%d/%d %d/%d %d/%d", &newFace.vertIndex[0], &newFace.coordIndex[0],
                                                   &newFace.vertIndex[1], &newFace.coordIndex[1],
                                                   &newFace.vertIndex[2], &newFace.coordIndex[2]);
    } else {
        fscanf(m_FilePointer, "%d %d %d", &newFace.vertIndex[0],
                                          &newFace.vertIndex[1],
                                          &newFace.vertIndex[2]);
    }

    fgets(strLine, 100, m_FilePointer);

    m_pFaces.push_back(newFace);

    m_bJustReadAFace = true;
}

void objLoader::fillInObjectInfo(t3DModel *pModel) {
    t3DObject newObject = {0};
    int textureOffset = 0, vertexOffset = 0;
    int i = 0;

    pModel->numOfObjects++;

    pModel->pObject.push_back(newObject);

    t3DObject *pObject = &(pModel->pObject[pModel->numOfObjects - 1]);

    pObject->numOfFaces   = m_pFaces.size();
    pObject->numOfVerts   = m_pVertices.size();
    pObject->numTexVertex = m_pTextureCoords.size();

    if(pObject->numOfFaces) {
        pObject->pFaces = new tFace[pObject->numOfFaces];
    }

    if(pObject->numOfVerts) {
        pObject->pVerts = new QVector3D[pObject->numOfVerts];
    }

    if(pObject->numTexVertex) {
        pObject->pTexVerts = new QVector2D[pObject->numTexVertex];
        pObject->bHasTexture = true;
    }

    for(i = 0; i < pObject->numOfFaces; i++) {
        pObject->pFaces[i] = m_pFaces[i];

        if(i == 0) {
            if(pObject->pFaces[0].vertIndex[0] != 1) {
                vertexOffset = pObject->pFaces[0].vertIndex[0] - 1;

                if(pObject->numTexVertex > 0) {
                    textureOffset = pObject->pFaces[0].coordIndex[0] - 1;
                }
            }
        }

        for(int j = 0; j < 3; j++) {
            pObject->pFaces[i].vertIndex[j]  -= 1 + vertexOffset;
            pObject->pFaces[i].coordIndex[j] -= 1 + textureOffset;
        }
    }

    for(i = 0; i < pObject->numOfVerts; i++) {
        pObject->pVerts[i] = m_pVertices[i];
    }

    for(i = 0; i < pObject->numTexVertex; i++) {
        pObject->pTexVerts[i] = m_pTextureCoords[i];
    }

    pObject->materialID = -1;

    m_pVertices.clear();
    m_pFaces.clear();
    m_pTextureCoords.clear();

    m_bObjectHasUV   = false;
    m_bJustReadAFace = false;
}


void objLoader::setObjectMaterial(t3DModel *pModel, int whichObject, int materialID) {
    if(!pModel) return;

    if(whichObject >= pModel->numOfObjects) return;

    pModel->pObject[whichObject].materialID = materialID;
}


void objLoader::addMaterial(t3DModel *pModel, char *strName, char *strFile, int r, int g, int b) {
    tMaterialInfo newMaterial = {0};

    newMaterial.color[0] = r; newMaterial.color[1] = g; newMaterial.color[2] = b;

    if(strFile) {
        strcpy(newMaterial.strFile, strFile);
    }

    if(strName) {
        strcpy(newMaterial.strName, strName);
    }

    pModel->pMaterials.push_back(newMaterial);

    pModel->numOfMaterials++;
}


QVector3D vector(QVector3D vPoint1, QVector3D vPoint2) {
    QVector3D vVector;

    vVector.setX(vPoint1.x() - vPoint2.x());
    vVector.setY(vPoint1.y() - vPoint2.y());
    vVector.setZ(vPoint1.z() - vPoint2.z());

    return vVector;
}

void objLoader::computeNormals(t3DModel *pModel) {
    QVector3D vVector1, vVector2, vNormal, vPoly[3];

    if(pModel->numOfObjects <= 0)
        return;

    for(int index = 0; index < pModel->numOfObjects; index++) {
        t3DObject *pObject = &(pModel->pObject[index]);

        QVector3D *pNormals      = new QVector3D[pObject->numOfFaces];
        QVector3D *pTempNormals  = new QVector3D[pObject->numOfFaces];
        pObject->pNormals        = new QVector3D[pObject->numOfVerts];

        for(int i=0; i < pObject->numOfFaces; i++) {
            vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
            vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
            vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];


            vVector1 = vector(vPoly[0], vPoly[2]);
            vVector2 = vector(vPoly[2], vPoly[1]);

            vNormal  = QVector3D::crossProduct(vVector1, vVector2);
            pTempNormals[i] = vNormal;
            vNormal  = vNormal.normalized();

            pNormals[i] = vNormal;
        }

        QVector3D vSum;
        QVector3D vZero = vSum;
        int shared = 0;

        for (int i = 0; i < pObject->numOfVerts; i++) {
            for (int j = 0; j < pObject->numOfFaces; j++) {
                if (pObject->pFaces[j].vertIndex[0] == i ||
                    pObject->pFaces[j].vertIndex[1] == i ||
                    pObject->pFaces[j].vertIndex[2] == i)
                {
                    vSum = vSum + pTempNormals[j];
                    shared++;
                }
            }

            pObject->pNormals[i] = vSum / float(-shared);

            pObject->pNormals[i] = pObject->pNormals[i].normalized();

            vSum = vZero;
            shared = 0;
        }

        delete [] pTempNormals;
        delete [] pNormals;
    }
}
