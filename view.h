#ifndef VIEW_H
#define VIEW_H

// To load the shaders
#include "lib/ResourceLoader.h"

#include <qgl.h>
#include <QTime>
#include <QTimer>

// GL Helper Library
//#include <glhlib.h>
#include <glhlib_2_1_win/source/TCylinder.h>

#include "Common.h"
#include "camera.h"
#include "skybox.h"
#include "treemaker.h"
#include <deque>

/*
 * Data for lights in a scene
 * From CS123SceneData.h in the projects
 */
#define MAX_NUM_LIGHTS 10
// Enumeration for light types.
enum LightType {
    LIGHT_POINT, LIGHT_DIRECTIONAL, LIGHT_SPOT, LIGHT_AREA
};
// Struct for a single light
struct CS123SceneLightData
{
   int id;
   LightType type;

   float color[4];
   glm::vec3 function;  // Attenuation function

   glm::vec4 pos;       // Not applicable to directional lights
   glm::vec4 dir;       // Not applicable to point lights

   float radius;        // Only applicable to spot lights
   float penumbra;      // Only applicable to spot lights
   float angle;         // Only applicable to spot lights

   float width, height; // Only applicable to area lights
};

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

    // Initilization functions
    void loadShaders();
    void makeCylinder();

    // Track if we have initilized or not
    bool m_OpenGLDidInit;

    // Have a single representative cylinder for the scene
    glhCylinderObjectf2 m_cylinder;
    void initCylinder();

    // Camera movement
    void moveCamera(const float &seconds);
    void translateCamera(const float &seconds);
    void rotateCamera(const float &seconds);

    Camera* m_camera;

    // Lighting functions
    void clearLights();
    void setLight(const CS123SceneLightData &light);

    // Texture loader
    GLuint loadTexture(std::string filename);

    // The ID of the main vao used for drawing
    GLuint m_vaoID;
    // The buffer with our vertices
    GLuint m_vertexBuffer;
    // The index buffer object
    GLuint m_IBO;
    // The id of the tree's texture
    GLuint m_pineTexID;
    // The id of the tree's normal map
    GLuint m_pineNormalMapID;

    // The program ID of the OpenGL shader
    GLuint m_shader;

    // A mapping of strings to their associated uniform locations in the shader
    std::map<std::string, GLint> m_uniformLocs;

    // A mapping of Qt keys and if they are pressed or not
    std::map<int, bool> m_keys;

    // For the skybox
    Skybox *m_skybox;

    // For the tree maker
    std::deque<glm::mat4x4> *m_treeBranches;
    std::deque<glm::mat4x4> *m_treeLeaves;
    TreeMaker m_treemaker;

    void generateTree();
    void reloadTree();

private slots:
    void tick();
};

#endif // VIEW_H

