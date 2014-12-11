#ifndef VIEW_H
#define VIEW_H

// To load the shaders
#include "lib/ResourceLoader.h"

#include <qgl.h>
#include <QTime>
#include <QTimer>

#include "Common.h"

#include "camera.h"

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    QTime time;
    QTimer timer;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void translateCamera(float seconds);

    Camera* m_camera;

    // The ID of the main vao used for drawing
    GLuint m_vaoID;

    // The program ID of the OpenGL shader
    GLuint m_shader;

    // A mapping of strings to their associated uniform locations in the shader
    std::map<std::string, GLint> m_uniformLocs;

    // A mapping of Qt keys and if they are pressed or not
    std::map<int, bool> m_keys;

private slots:
    void tick();
};

#endif // VIEW_H

