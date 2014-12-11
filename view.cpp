#include "view.h"
#include <QApplication>
#include <QKeyEvent>

View::View(QWidget *parent) : QGLWidget(parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    m_camera = new Camera();
}

View::~View()
{
    delete m_camera;
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    // Set up the OpenGL context with GLEW
    //initialize glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    glGetError(); // Clear errors after call to glewInit
    if (GLEW_OK != err)
    {
      // Problem: glewInit failed, something is seriously wrong.
      fprintf(stderr, "Error initializing glew: %s\n", glewGetErrorString(err));
    }

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    time.start();
    timer.start(1000 / 60);

    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));


    // Set up the shaders
    m_shader = ResourceLoader::loadShaders(
            ":/shaders/default.vert",
            ":/shaders/default.frag");

    m_uniformLocs["p"]= glGetUniformLocation(m_shader, "p");
    m_uniformLocs["m"]= glGetUniformLocation(m_shader, "m");
    m_uniformLocs["v"]= glGetUniformLocation(m_shader, "v");
    m_uniformLocs["allBlack"]= glGetUniformLocation(m_shader, "allBlack");
    m_uniformLocs["useLighting"]= glGetUniformLocation(m_shader, "useLighting");
    m_uniformLocs["ambient_color"] = glGetUniformLocation(m_shader, "ambient_color");
    m_uniformLocs["diffuse_color"] = glGetUniformLocation(m_shader, "diffuse_color");
    m_uniformLocs["specular_color"] = glGetUniformLocation(m_shader, "specular_color");
    m_uniformLocs["shininess"] = glGetUniformLocation(m_shader, "shininess");
    m_uniformLocs["useTexture"] = glGetUniformLocation(m_shader, "useTexture");
    m_uniformLocs["tex"] = glGetUniformLocation(m_shader, "tex");
    m_uniformLocs["useArrowOffsets"] = glGetUniformLocation(m_shader, "useArrowOffsets");
    m_uniformLocs["blend"] = glGetUniformLocation(m_shader, "blend");

    // For testing, draw a simple triangle to screen

    // Generate and bind the VAO
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // Generate and bind the VBO
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // Generate the triangle
    GLfloat vertexData[] = {
        -1, -1, 0, // Pos 1
        0, 0, 1, // Normal 1
        1, -1, 0, // Pos 2
        0, 0, 1, // Normal 2
        0, 1, 0, // Pos 3
        0, 0, 1, // Normal 3
        // START TRIANGLE TWO
        0, 0, 0,
        0, 0, 1,
        5, 0, 0,
        0, 0, 1,
        5, 5, 0,
        0, 0, 1
    };

    // Pass the vertex data to OpenGL
    glBufferData(GL_ARRAY_BUFFER, 6 * 6 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    // Tell the VAO about this buffer
    glEnableVertexAttribArray(glGetAttribLocation(m_shader, "position"));
    glEnableVertexAttribArray(glGetAttribLocation(m_shader, "normal"));
    glVertexAttribPointer(
                glGetAttribLocation(m_shader, "position"),
                3, // Num coords per position
                GL_FLOAT, // Type of data
                GL_FALSE, // Normalized?
                6 * sizeof(GLfloat), // Stride between entries
                (void *)0 // Start location offset in the buffer
                );
    glVertexAttribPointer(
                glGetAttribLocation(m_shader, "normal"),
                3, // Num coords per position
                GL_FLOAT, // Type of data
                GL_TRUE, // Normalized?
                6 * sizeof(GLfloat), // Stride between entries
                (void *)(sizeof(GLfloat) * 3) // Start location offset in the buffer
                );

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}

void View::paintGL()
{
    // Draw a grey background so we can see unlight objects
    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Implement the demo rendering here

    // Use the shader program
    glUseProgram(m_shader);

    // Set the uniforms
    glUniform1i(m_uniformLocs["useLighting"], true);
    glUniform1i(m_uniformLocs["useArrowOffsets"], GL_FALSE);
    glUniformMatrix4fv(m_uniformLocs["p"], 1, GL_FALSE,
            glm::value_ptr(m_camera->getProjectionMatrix()));
    glUniformMatrix4fv(m_uniformLocs["v"], 1, GL_FALSE,
            glm::value_ptr(m_camera->getViewMatrix()));
    glUniformMatrix4fv(m_uniformLocs["m"], 1, GL_FALSE,
            glm::value_ptr(glm::mat4()));
    glUniform3f(m_uniformLocs["allBlack"], 1, 1, 1);

    // Apply the default material for an object
    // All are specified in RGB order
    float ambient[3] = {0.2f, 0.1f, 0.0f};
    float diffuse[3] = {1.0f, 0.5f, 0.0f};
    float specular[3] = {1.0f, 1.0f, 1.0f};
    float shininess = 64.0f;

    glUniform3fv(m_uniformLocs["ambient_color"], 1, ambient);
    glUniform3fv(m_uniformLocs["diffuse_color"], 1, diffuse);
    glUniform3fv(m_uniformLocs["specular_color"], 1, specular);
    glUniform1f(m_uniformLocs["shininess"], shininess);
    /*
     * Don't use textures yet
    if (material.textureMap && material.textureMap->isUsed && material.textureMap->texid) {
        glUniform1i(m_uniformLocs["useTexture"], 1);
        glUniform1i(m_uniformLocs["tex"], 1);
        glUniform1f(m_uniformLocs["blend"], 1.0f);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.textureMap->texid);
        glActiveTexture(GL_TEXTURE0);
    } else {
    */

    // Don't use textures yet
    glUniform1i(m_uniformLocs["useTexture"], 0);

    // Draw the example triangle to screen
    glBindVertexArray(m_vaoID);

    // TODO: apply a material and use a camera to pass variables to the shader
    // That way we actually draw something
    // Can set color in fragment shader for debugging
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Unbind the shader
    glUseProgram(m_shader);
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    int deltaX = event->x() - width() / 2;
    int deltaY = event->y() - height() / 2;
    if (!deltaX && !deltaY) return;
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    // TODO: Handle mouse movements here

    // Update the camera's rotation based on mouse movements
    float cameraRotation = 1.0f / 16.0f;

    m_camera->rotateU(cameraRotation * deltaY);
    m_camera->rotateV(cameraRotation * deltaX);

}

void View::mouseReleaseEvent(QMouseEvent *event)
{
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // Set the key as pressed
    m_keys[event->key()] = true;
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    // Unset the key
    m_keys[event->key()] = false;
}

/**
 * @brief View::moveCamera
 * @param seconds number of seconds since the last tick (frame update)
 * Updates the camera's location based on the pressed keys and the passsed number of seconds
 */
void View::moveCamera(const float& seconds)
{
    // Move the camera along the xz plane
    translateCamera(seconds);

    // Rotate the camera around the y axis
    rotateCamera(seconds);
}

/**
 * @brief View::translateCamera moves the camera along the xz plane
 * @param seconds number of seconds since the last tick (frame update)
 */
void View::translateCamera(const float& seconds)
{
    glm::vec4 vec = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    // Speed of the camera's movement
    float movementSpeed = seconds * 2.0f;

    // W and S translate the z plane
    if(m_keys[Qt::Key_W])
    {
        vec.z -= movementSpeed;
    }
    if(m_keys[Qt::Key_S])
    {
        vec.z += movementSpeed;
    }
    // A and D translate the x plane
    if(m_keys[Qt::Key_A])
    {
        vec.x -= movementSpeed;
    }
    if(m_keys[Qt::Key_D])
    {
        vec.x += movementSpeed;
    }

    m_camera->translate(vec);
}

/**
 * @brief View::translateCamera moves the camera along the xz plane
 * @param seconds number of seconds since the last tick (frame update)
 */
void View::rotateCamera(const float& seconds)
{
    float degrees = seconds * 15.0f;
    if(m_keys[Qt::Key_Q])
    {
        m_camera->rotateV(degrees);
    }
    if(m_keys[Qt::Key_E])
    {
        m_camera->rotateV(-degrees);
    }
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;

    // TODO: Implement the demo update here

    // Move the camera
    moveCamera(seconds);

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
