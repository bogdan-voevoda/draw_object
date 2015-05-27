#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <QVector3D>
#include <QVector2D>

// This is our face structure.  This is is used for indexing into the vertex
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace {
    int vertIndex[3];                       // indicies for the verts that make up this triangle
    int coordIndex[3];                      // indicies for the tex coords to texture this face
};

//Material info
struct tMaterialInfo {
    char  strName[255];                     // The texture name
    char  strFile[255];                     // The texture file name (If this is set it's a texture map)
    unsigned char  color[3];                // The color of the object (R, G, B)
    int   texureId;                         // the texture ID
    float uTile;                            // u tiling of texture  (Currently not used)
    float vTile;                            // v tiling of texture  (Currently not used)
    float uOffset;                          // u offset of texture  (Currently not used)
    float vOffset;                          // v offset of texture  (Currently not used)
} ;

//Object information, objects into OBJ
struct t3DObject {
    int  numOfVerts;                        // The number of verts in the model
    int  numOfFaces;                        // The number of faces in the model
    int  numTexVertex;                      // The number of texture coordinates
    int  materialID;                        // The texture ID to use, which is the index into our texture array
    bool bHasTexture;                       // This is TRUE if there is a texture map for this object
    char strName[255];                      // The name of the object
    QVector3D  *pVerts;                      // The object's vertices
    QVector3D  *pNormals;                    // The object's normals
    QVector2D  *pTexVerts;                   // The texture's UV coordinates
    tFace *pFaces;                          // The faces information of the object
};

//Model information, OBJ Container (Model will can have one or more objects)
struct t3DModel {
    t3DModel():numOfObjects(0),numOfMaterials(0){}
    int numOfObjects;                                    // The number of objects in the model
    int numOfMaterials;                                  // The number of materials for the model
    std::vector<tMaterialInfo> pMaterials;               // The list of material information (Textures and colors)
    std::vector<t3DObject> pObject;                      // The object list for our model
};


class objLoader {

public:
    objLoader(): m_bObjectHasUV(false), m_bJustReadAFace(false) {}

    // This will be the only function that needs to be called by you.
    // Just pass in your model that will hold the information and the file name.
    bool importObj(t3DModel *pModel, char *strFileName);

    // This is the main loading loop that gets called in ImportObj()
    void readObjFile(t3DModel *pModel);

    // This is called in ReadObjFile() if we find a line starting with 'v'
    void readVertexInfo();

    // This is called in ReadObjFile() if we find a line starting with 'f'
    void readFaceInfo();

    // This is called when we are finished loading in the face information
    void fillInObjectInfo(t3DModel *pModel);

    // This isn't necessary for the loader, but it's nice to have vertex normals for lighting
    void computeNormals(t3DModel *pModel);

    // Since .obj files don't have texture/material names, we create a function to set them manually.
    // The materialID is the index into the pMaterial array for our model.
    void setObjectMaterial(t3DModel *pModel, int whichObject, int materialID);

    // To make it easier to assign a material to a .obj object we create a funtion to do so.
    // We can pass in the model, the material name, the texture file name and the RGB colors.
    // If we just want a color, pass in NULL for strFile.
    void addMaterial(t3DModel *pModel, char *strName, char *strFile, int r = 255, int g = 255, int b = 255);
    // This tells us if the current object has texture coordinates
    bool m_bObjectHasUV;

    // This tells us if we just read in face data so we can keep track of multiple objects
    bool m_bJustReadAFace;

private:

    // This holds our file pointer for reading in the file
    FILE *m_FilePointer;

    //vector that holds a list of vertices
    std::vector<QVector3D>  m_pVertices;

    //vector that holds a list of faces
    std::vector<tFace> m_pFaces;

    //vector that holds a list of UV Coordinates
    std::vector<QVector2D>  m_pTextureCoords;
};

#endif // OBJLOADER_H
