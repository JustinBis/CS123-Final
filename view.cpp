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
}

View::~View()
{
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

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
        0, 0, 1 // Normal 3
    };

    // Pass the vertex data to OpenGL
    glBufferData(GL_ARRAY_BUFFER, 3 * 6 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Implement the demo rendering here

    // Draw the example triangle to screen
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
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
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here

    // Look for wasd keys for camera movement
}

void View::keyReleaseEvent(QKeyEvent *event)
{

    // Unset the camera movement
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;

    // TODO: Implement the demo update here

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
