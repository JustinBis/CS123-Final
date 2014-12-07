#ifndef VIEW_H
#define VIEW_H

// To load the shaders
#include "lib/ResourceLoader.h"

#include <qgl.h>
#include <QTime>
#include <QTimer>

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

    // The ID of the main vao used for drawing
    GLuint m_vaoID;

    // The program ID of the OpenGL shader
    GLuint m_shader;

    // A mapping of strings to their associated uniform locations in the shader
    std::map<std::string, GLint> m_uniformLocs;

private slots:
    void tick();
};

#endif // VIEW_H

