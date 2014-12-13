#include "skybox.h"

#include <QFile>
#include <qgl.h>
#include "ResourceLoader.h"

Skybox::Skybox()
{
    // Load the shader
    std::cout << "Loading skybox shader" << std::endl;
    loadShader();

    // Load the cubemap
    std::cout << "Loading skybox cubemap" << std::endl;
    createCubemap();

    // Load the vertex buffer object
    std::cout << "Loading skybox vertex buffer" << std::endl;
    loadBuffer();
}


Skybox::~Skybox()
{
    // Delete the VAO and VBO
    glDeleteBuffers(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);

    // Delete the texture
    glDeleteTextures(1, &m_texture);

}

void Skybox::draw(Camera *camera)
{
    // Disable the depth mask
    glDepthMask(GL_FALSE);

    // Use the shader
    glUseProgram(m_shader);

    // Give the shader V and P
    glm::mat4 V = camera->getViewMatrix();
    glm::mat4 P = camera->getProjectionMatrix();

    GLuint Vloc = glGetUniformLocation(m_shader, "V");
    GLuint Ploc = glGetUniformLocation(m_shader, "P");

    // Take the camera translation out of the view matrix
    V = V - (V * glm::mat4(
                0.0f,  0.0f,  0.0f, 0.0f,
                0.0f,  0.0f,  0.0f, 0.0f,
                0.0f,  0.0f,  0.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 0.0f
                ));

    glUniformMatrix4fv(
                Vloc, // Shader variable
                1, // Number of matricies
                GL_FALSE, //
                glm::value_ptr(V) // Pointer to the first element
            );
    glUniformMatrix4fv(
                Ploc, // Shader variable
                1, // Number of matricies
                GL_FALSE, //
                glm::value_ptr(P) // Pointer to the first element
            );

    // Set up the texture
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_CUBE_MAP, m_texture);

    glBindVertexArray (m_vao);
    glDrawArrays (GL_TRIANGLES, 0, 36);

    // Unbind the shader
    glUseProgram(0);

    // Reenable the depth mask
    glDepthMask(GL_TRUE);
}

void Skybox::loadShader()
{
    m_shader = ResourceLoader::loadShaders(
            ":/shaders/skybox.vert",
            ":/shaders/skybox.frag");
}

void Skybox::loadBuffer()
{
    // Points for a cube
    float points[] = {
      -25.0f,  25.0f, -25.0f,
      -25.0f,  -25.0f, -25.0f,
       25.0f,  -25.0f, -25.0f,
       25.0f,  -25.0f, -25.0f,
       25.0f,  25.0f, -25.0f,
      -25.0f,  25.0f, -25.0f,

      -25.0f,  -25.0f, 25.0f,
      -25.0f,  -25.0f, -25.0f,
      -25.0f,  25.0f, -25.0f,
      -25.0f,  25.0f, -25.0f,
      -25.0f,  25.0f,  25.0f,
      -25.0f,  -25.0f, 25.0f,

       25.0f,  -25.0f, -25.0f,
       25.0f,  -25.0f, 25.0f,
       25.0f,  25.0f,  25.0f,
       25.0f,  25.0f,  25.0f,
       25.0f,  25.0f, -25.0f,
       25.0f,  -25.0f, -25.0f,

      -25.0f,  -25.0f, 25.0f,
      -25.0f,  25.0f,  25.0f,
       25.0f,  25.0f,  25.0f,
       25.0f,  25.0f,  25.0f,
       25.0f,  -25.0f, 25.0f,
      -25.0f,  -25.0f, 25.0f,

      -25.0f,  25.0f, -25.0f,
       25.0f,  25.0f, -25.0f,
       25.0f,  25.0f,  25.0f,
       25.0f,  25.0f,  25.0f,
      -25.0f,  25.0f,  25.0f,
      -25.0f,  25.0f, -25.0f,

      -25.0f,  -25.0f, -25.0f,
      -25.0f,  -25.0f, 25.0f,
       25.0f,  -25.0f, -25.0f,
       25.0f,  -25.0f, -25.0f,
      -25.0f,  -25.0f, 25.0f,
       25.0f,  -25.0f, 25.0f
    };

    glGenBuffers (1, &m_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
    glBufferData (GL_ARRAY_BUFFER, 3 * 36 * sizeof (float), &points, GL_STATIC_DRAW);

    glGenVertexArrays (1, &m_vao);
    glBindVertexArray (m_vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Unbind the buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint Skybox::createCubemap()
{
    // generate a cube-map texture to hold all the sides
    glActiveTexture (GL_TEXTURE0);
    glGenTextures(1, &m_texture);

      // load each image and copy into a side of the cube-map texture
      assert (
        load_cube_map_side (m_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, ":/textures/mp_organic/organic_ft.png"));
      assert (
        load_cube_map_side (m_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, ":/textures/mp_organic/organic_bk.png"));
      assert (
        load_cube_map_side (m_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, ":/textures/mp_organic/organic_up.png"));
      assert (
        load_cube_map_side (m_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, ":/textures/mp_organic/organic_dn.png"));
      assert (
        load_cube_map_side (m_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, ":/textures/mp_organic/organic_lf.png"));
      assert (
        load_cube_map_side (m_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, ":/textures/mp_organic/organic_rt.png"));
      // format cube map texture
      glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool Skybox::load_cube_map_side(GLuint texture, GLenum side_target, std::string filename)
{
  glBindTexture (GL_TEXTURE_CUBE_MAP, texture);

  int x, y, n;
  int force_channels = 4;

  // Load the image data
  QString qfilename = QString::fromStdString(filename);
  // Make sure the image file exists
  QFile file(qfilename);
  if (!file.exists())
  {
      std::cerr << "Warning: loading texture failed. File: " << filename << std::endl;
      return -1;
  }

  // Load the file into memory
  QImage image;
  image.load(file.fileName());
  image = image.mirrored(false, true);

  // Mirror the ground and ceiling
  if(side_target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y || side_target == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
  {
      image = image.mirrored(false, false);
  }

  QImage textureData = QGLWidget::convertToGLFormat(image);

  unsigned char*  image_data = textureData.bits();
  x = textureData.width();
  y = textureData.height();

  if (!image_data) {
    std::cerr << "ERROR: could not load " << filename << std::endl;
    return false;
  }

  // non-power-of-2 dimensions check
  if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
    std::cerr << "WARNING: image " << filename << " is not power-of-2 dimensions" << std::endl;
  }

  // copy image data into 'target' side of cube map
  glTexImage2D (
    side_target,
    0,
    GL_RGBA,
    x,
    y,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    image_data
  );

  return true;
}
