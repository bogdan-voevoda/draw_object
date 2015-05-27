#include "render_object.h"
#include <QOpenGLFramebufferObject>

#include <QGLFramebufferObject>

render_object::render_object(MainWidget *parent) : parent(parent) {
    initializeGLFunctions();

    glGenBuffers(2, vbo);

    isVisiblse = true;

    light.ambient = QVector4D(0.2f, 0.2f, 0.2f, 1.0f);
    light.attenuation = QVector3D(0.5f, 0.0f, 0.0000005f);
    light.diffuse = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
    light.position = QVector4D(0.0f, 300.0f, 0.0f, 1.0f);
    light.specular = QVector4D(1.0f, 1.0f, 1.0f, 100.0f);

    // INIT SHADER PROGRAM

    QGLShaderProgram *program1 = new QGLShaderProgram();
    QString vertShaderlocation = ":/light_models/vert.glsl";
    QString fragShaderlocation = ":/light_models/frag.glsl";
    initShaders(vertShaderlocation, fragShaderlocation, *program1);

    shader_programs.push_back(program1);

    // CREATE TERRAIN

    uint globalIndexer = 0;

    GLuint terrainTexture[1];

    QString terTexStr = ":/terrain/Terrain.bmp";

    terrainTexture[0] = initTexture(terTexStr);

    terrain *ground = new terrain(0, 0, 0, terrainTexture, globalIndexer);

    QString terrLocationStr = ":/terrain/TerrainHeightmap.raw";

    ground->loadRawFile(terrLocationStr, &ground->vertices, &ground->normals, &ground->texCoord, &ground->indices);

    globalIndexer += ground->indices.size();

    ground->shaderProgram = program1;

    draw_objects.push_back(ground);

    // CREATE SKYBOX

    GLuint skyBoxTextures[6];

    QString top = ":/sky_box/Top.bmp";
    QString front = ":/sky_box/Front.bmp";
    QString back = ":/sky_box/Back.bmp";
    QString left = ":/sky_box/Left.bmp";
    QString right = ":/sky_box/Right.bmp";
    QString bottom = ":/sky_box/Bottom.bmp";

    skyBoxTextures[0] = initTexture(bottom);
    skyBoxTextures[1] = initTexture(top);
    skyBoxTextures[2] = initTexture(front);
    skyBoxTextures[3] = initTexture(back);
    skyBoxTextures[4] = initTexture(left);
    skyBoxTextures[5] = initTexture(right);

    skybox *sky = new skybox(0, 0, 0, 5000, 5000, 5000, skyBoxTextures, globalIndexer);

    sky->shaderProgram = program1;

    globalIndexer += sky->indices.size();

    draw_objects.push_back(sky);

    // INIT NEW CREATED OBJECTS IN VBO

    initGeometry(vbo);
}

render_object::~render_object() {
    glDeleteBuffers(2, vbo);

    uint border = draw_objects.size();
    for (uint i = 0; i < border; ++i) {
        delete draw_objects.at(i);
    }
    draw_objects.clear();

    border = draw_models.size();
    for (uint i = 0; i < border; ++i) {
        delete draw_models.at(i);
    }
    draw_models.clear();

    border = shader_programs.size();
    for (uint i = 0; i < border; ++i) {
        delete shader_programs.at(i);
    }
    shader_programs.clear();
}

void render_object::initShaders(const QString &vertSaderLocation, const QString &fragSaderLocation, QGLShaderProgram &program) {
    if (!program.addShaderFromSourceFile(QGLShader::Vertex, vertSaderLocation)) {
        qDebug() << "Error while openning vertex shader file with adress: " << vertSaderLocation;
        parent->getParent()->close();
    }
    if (!program.addShaderFromSourceFile(QGLShader::Fragment, fragSaderLocation)) {
        qDebug() << "Error while openning fragment shader file with adress: " << fragSaderLocation;
        parent->getParent()->close();
    }
    if (!program.link()) {
        qDebug() << "Error while linking shaders with adress: " << vertSaderLocation << fragSaderLocation;
        parent->getParent()->close();
    }
}

GLuint render_object::initTexture(const QString &textureLocation) {
    QImage img(textureLocation);

    if (img.isNull()) {
        qDebug() << "Error while opening texture file with adress: " << textureLocation;
        return NULL;
    } else {
        GLuint id;

        glGenTextures(1, &id);
        glEnable(GL_TEXTURE_2D);
        id = parent->bindTexture(img);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // check for android
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        return id;
    }
}

void render_object::initGeometry(GLuint *vbo) {
    uint draw_objects_quantity = draw_objects.size();

    uint verticesQuantity = 0;
    uint normalsQuantity = 0;
    uint texCoordQuantity = 0;
    uint indicesQuantity = 0;

    for (uint i = 0; i < draw_objects_quantity; ++i) {
        verticesQuantity += draw_objects[i]->vertices.size();
        normalsQuantity += draw_objects[i]->normals.size();
        texCoordQuantity += draw_objects[i]->texCoord.size();
        indicesQuantity += draw_objects[i]->indices.size();
    }

    uint sizeOf3DVector = sizeof(QVector3D);
    uint sizeOf2DVector = sizeof(QVector2D);

    uint verticesSize = sizeOf3DVector * verticesQuantity;
    uint normalsSize  = sizeOf3DVector * normalsQuantity;
    uint texCoordSize = sizeOf2DVector * texCoordQuantity;
    uint indicesSize  = sizeof(GLuint) * indicesQuantity;

    uint vertexDataSize = verticesSize + normalsSize + texCoordSize;

    uint vertexOffset = 0;
    uint indOffset = 0;

    qDebug() << "VERTICES" << verticesQuantity << verticesSize;
    qDebug() << "NORMALS" << normalsQuantity << normalsSize;
    qDebug() << "TEXCOORD" << texCoordQuantity << texCoordSize;
    qDebug() << "INDICES" << indicesQuantity << indicesSize;


    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);


    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, NULL, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, NULL, GL_STATIC_DRAW);

    for (uint j = 0; j < draw_objects_quantity; ++j) {
        draw_object *pTempObj = draw_objects[j];

        verticesQuantity = pTempObj->vertices.size();

        for (uint i = 0; i < verticesQuantity; ++i) {
             glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeOf3DVector, &pTempObj->vertices[i]); // ADD VERTEX
             vertexOffset += sizeOf3DVector;

             glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeOf3DVector, &pTempObj->normals[i]);  // ADD NORMAL
             vertexOffset += sizeOf3DVector;

             glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, sizeOf2DVector, &pTempObj->texCoord[i]); // ADD UV
             vertexOffset += sizeOf2DVector;
        }

        uint tempIndQuantity = pTempObj->indices.size();

        pTempObj->positionInVBO = indOffset;

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indOffset, tempIndQuantity * sizeof(GLuint), pTempObj->indices.data()); // ADD INDICES

        indOffset += (tempIndQuantity)* sizeof(GLuint);
    }
}

void render_object::bindGeometry(QGLShaderProgram *program) {
//    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);

    quintptr offset = 0;

    uint size = sizeof(QVector3D) * 2 + sizeof(QVector2D);

    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, size, (const void*)offset);

    offset += sizeof(QVector3D);

    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, size, (const void*)offset);

    offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, size, (const void *)offset);

//    glDrawElements(mode, indexQuantity, GL_UNSIGNED_INT, (void*)(startOffset));
}

void render_object::drawObjectWithProgram(QGLShaderProgram *program, draw_object *obj) {

}

void render_object::createObject() {

}

void render_object::addVertices(std::vector<QVector3D> *vertices, std::vector<QVector3D> *normals, std::vector<QVector2D> *texCoord, std::vector<GLuint> *indices, uint &indexer, uint *sides, float x, float y, float z, float size) {
    x -= size/2;
    y -= size/2;
    z += size/2;

    if (sides[0]) {
        vertices->push_back(QVector3D(x,      y, z));
        vertices->push_back(QVector3D(x,      y, z-size));
        vertices->push_back(QVector3D(x+size, y, z-size));
        vertices->push_back(QVector3D(x+size, y, z));

        normals->push_back(QVector3D(0, -1,0));
        normals->push_back(QVector3D(0, -1,0));
        normals->push_back(QVector3D(0, -1,0));
        normals->push_back(QVector3D(0, -1,0));

        texCoord->push_back(QVector2D(0.0f, 0.0f));
        texCoord->push_back(QVector2D(1, 0.0f));
        texCoord->push_back(QVector2D(1, 1));
        texCoord->push_back(QVector2D(0.0f, 1));

        indexSeter(indices, indexer);
    }
    if (sides[1]) {
        vertices->push_back(QVector3D(x,      y+size, z)    );
        vertices->push_back(QVector3D(x+size, y+size, z)    );
        vertices->push_back(QVector3D(x+size, y+size, z-size));
        vertices->push_back(QVector3D(x,      y+size, z-size));

        normals->push_back(QVector3D(0, 1,0));
        normals->push_back(QVector3D(0, 1,0));
        normals->push_back(QVector3D(0, 1,0));
        normals->push_back(QVector3D(0, 1,0));

        texCoord->push_back(QVector2D(0.0f, 0.0f));
        texCoord->push_back(QVector2D(1,    0.0f));
        texCoord->push_back(QVector2D(1,    1.0f));
        texCoord->push_back(QVector2D(0.0f, 1.0f));

        indexSeter(indices, indexer);
    }
    if (sides[2]) {
        vertices->push_back(QVector3D(x,      y,      z));
        vertices->push_back(QVector3D(x+size, y,      z));
        vertices->push_back(QVector3D(x+size, y+size, z));
        vertices->push_back(QVector3D(x,      y+size, z));

        normals->push_back(QVector3D(0, 0,1));
        normals->push_back(QVector3D(0, 0,1));
        normals->push_back(QVector3D(0, 0,1));
        normals->push_back(QVector3D(0, 0,1));

        texCoord->push_back(QVector2D(0.0f, 0.0f));
        texCoord->push_back(QVector2D(1, 0.0f));
        texCoord->push_back(QVector2D(1, 1));
        texCoord->push_back(QVector2D(0.0f, 1));

        indexSeter(indices, indexer);
    }
    if (sides[3]) {
        vertices->push_back(QVector3D(x+size, y,      z-size));
        vertices->push_back(QVector3D(x,      y,      z-size));
        vertices->push_back(QVector3D(x,      y+size, z-size));
        vertices->push_back(QVector3D(x+size, y+size, z-size));

        normals->push_back(QVector3D(0, 0,-1));
        normals->push_back(QVector3D(0, 0,-1));
        normals->push_back(QVector3D(0, 0,-1));
        normals->push_back(QVector3D(0, 0,-1));

        texCoord->push_back(QVector2D(0.0f, 0.0f));
        texCoord->push_back(QVector2D(1, 0.0f));
        texCoord->push_back(QVector2D(1, 1));
        texCoord->push_back(QVector2D(0.0f, 1));

        indexSeter(indices, indexer);
    }
    if (sides[4]) {
        vertices->push_back(QVector3D(x+size, y,      z)     );
        vertices->push_back(QVector3D(x+size, y,      z-size));
        vertices->push_back(QVector3D(x+size, y+size, z-size));
        vertices->push_back(QVector3D(x+size, y+size, z)     );

        normals->push_back(QVector3D(1, 0,0));
        normals->push_back(QVector3D(1, 0,0));
        normals->push_back(QVector3D(1, 0,0));
        normals->push_back(QVector3D(1, 0,0));

        texCoord->push_back(QVector2D(0.0f, 0.0f));
        texCoord->push_back(QVector2D(1, 0.0f));
        texCoord->push_back(QVector2D(1, 1));
        texCoord->push_back(QVector2D(0.0f, 1));

        indexSeter(indices, indexer);
    }
    if (sides[5]) {
        vertices->push_back(QVector3D(x,      y,      z-size));
        vertices->push_back(QVector3D(x,      y,      z)     );
        vertices->push_back(QVector3D(x,      y+size, z)     );
        vertices->push_back(QVector3D(x,      y+size, z-size));

        normals->push_back(QVector3D(-1, 0,0));
        normals->push_back(QVector3D(-1, 0,0));
        normals->push_back(QVector3D(-1, 0,0));
        normals->push_back(QVector3D(-1, 0,0));

        texCoord->push_back(QVector2D(0.0f, 0.0f));
        texCoord->push_back(QVector2D(1, 0.0f));
        texCoord->push_back(QVector2D(1, 1));
        texCoord->push_back(QVector2D(0.0f, 1));

        indexSeter(indices, indexer);
    }
}

void render_object::indexSeter(std::vector<GLuint> *indices, uint &indexer) {
    indices->push_back(3+indexer);
    indices->push_back(0+indexer);
    indices->push_back(1+indexer);

    indices->push_back(1+indexer);
    indices->push_back(2+indexer);
    indices->push_back(3+indexer);

    indexer += 4;
}


bool bOnce = true;

void render_object::renderObject() {

    terrain* ground = static_cast<terrain*> (draw_objects.at(0));
    skybox* sky = static_cast<skybox*> (draw_objects.at(1));

    if (parent->getCamera()->isMoving) {
        int border = ground->vertices.size() - 2;
        for (int i = 0; i < border; i+=2) {

            QVector3D vec[3] = {QVector3D(ground->vertices.at(i)), QVector3D(ground->vertices.at(i+1)), QVector3D(ground->vertices.at(i+2))};

            QVector3D vec2[3] = {QVector3D(ground->vertices.at(i+1)), QVector3D(ground->vertices.at(i+2)), QVector3D(ground->vertices.at(i+3))};

            parent->getCamera()->checkCameraCollision(vec, 3);

            parent->getCamera()->checkCameraCollision(vec2, 3);
        }
    }

    if (bOnce) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);

        shader_programs[0]->bind();
        bindGeometry(shader_programs[0]);
        bOnce = !bOnce;
    }

    cameraSetup(shader_programs[0]);

    ground->draw();
    sky->draw();
}

void render_object::pointLightSetup(QGLShaderProgram &program, const PointLight &light) {
    program.setUniformValue("u_light_position", light.position);
    program.setUniformValue("u_light_ambient", light.ambient);
    program.setUniformValue("u_light_diffuse", light.diffuse);
    program.setUniformValue("u_light_specular", light.specular);
    program.setUniformValue("u_light_attenuation", light.attenuation);
}

void render_object::materialSetup(QGLShaderProgram &program, const Material &material) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.texture);
    program.setUniformValue("u_material_texture", 0);

    program.setUniformValue("u_material_ambient", material.ambient);

    program.setUniformValue("u_material_diffuse", material.diffuse);
    program.setUniformValue("u_material_specular", material.specular);
    program.setUniformValue("u_material_emission", material.emission);
    program.setUniformValue("u_material_shininess", material.shininess);
}

void render_object::cameraSetup(QGLShaderProgram *program) {
    QMatrix4x4 model;
    QMatrix4x4 view;
    view *= parent->getCamera()->GLRotation(parent->getCamera()->rotation);
    view.transposed();
    view.translate(-parent->getCamera()->position);

    QMatrix4x4 viewProjection = parent->getCamera()->projection * view;

    program->setUniformValue("u_transform_viewProjection", viewProjection);

    parent->getCamera()->calculateFrustum(viewProjection, model);
}

void render_object::objectRotation(QVector3D &objPos, float dt) {
    static float angle = 0;
    float speed = (2*math_pi) / 10; //2*PI in degress is 360, so you get 10 seconds to complete a circle
    float radius = 700;

    angle += speed * dt; //if you want to switch direction, use -= instead of +=

    float x = std::cos(angle)*radius;
    float y = std::sin(angle)*radius/2.5;
    float z = std::sin(angle)*radius;

    float xShift = 0;
    float yShift = 600;
    float zShift = 0;

    objPos.setX(xShift+x);
    objPos.setY(yShift+y);
    objPos.setZ(zShift+z);
}

