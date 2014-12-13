#ifndef SKYBOX_H
#define SKYBOX_H

#include "Common.h"
#include "camera.h"

class Skybox
{
public:
    Skybox();
    ~Skybox();

    // Draw the skybox
    void draw(Camera *camera);

private:

    void loadShader();
    void loadBuffer();
    GLuint createCubemap();
    bool load_cube_map_side(GLuint texture, GLenum side_target, std::string filename);

    // The program ID of the OpenGL shader
    GLuint m_shader;

    // VAO and VBO
    GLuint m_vao;
    GLuint m_vbo;

    // Texture ids
    GLuint m_texture;
    GLuint m_tex_bk;
    GLuint m_tex_dn;
    GLuint m_tex_ft;
    GLuint m_tex_lf;
    GLuint m_tex_rt;
    GLuint m_tex_up;
};

#endif // SKYBOX_H
