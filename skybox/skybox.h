#ifndef SKYBOX_H
#define SKYBOX_H

#include "draw_object.h"

class skybox : public draw_object {

public:
    skybox(float x, float y, float z, float width, float height, float length, GLuint *skyBoxTextures, uint ind_shift);
    ~skybox();
    void draw();
};

#endif // SKYBOX_H
