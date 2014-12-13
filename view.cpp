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

    m_treeBranches = new std::deque<glm::mat4x4>;
    m_treeLeaves = new std::deque<glm::mat4x4>;

    m_treemaker = TreeMaker();

    m_OpenGLDidInit = false;
}

View::~View()
{
    if(m_OpenGLDidInit)
    {
        // Delete the OpenGL buffers
        glDeleteVertexArrays(1, &m_vaoID);
        glDeleteBuffers(1, &m_vertexBuffer);

        // Delete the clinder
        glhDeleteCylinderf2(&m_cylinder);
    }

    delete m_treeBranches;
    delete m_treeLeaves;

    delete m_camera;
}

void View::initializeGL()
{
    std::cout << "Initilizing OpenGL" << std::endl;
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
    std::cout << "Loading Shaders" << std::endl;
    loadShaders();

    // Create the cylinder using the glh library
    std::cout << "Creating unit cylinder" << std::endl;
    initCylinder();

    std::cout << "Generating Buffers" << std::endl;

    // Generate and bind the VAO
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // Generate and bind the VBO
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

    // Buffer the cylinder data
    std::cout << "Buffering data" << std::endl;
    glBufferData(GL_ARRAY_BUFFER, m_cylinder.VertexCount * sizeof(GLHVertex_VNT), m_cylinder.pVertex, GL_STATIC_DRAW);

    // Tell the VAO about this buffer
    glEnableVertexAttribArray(glGetAttribLocation(m_shader, "position"));
    glEnableVertexAttribArray(glGetAttribLocation(m_shader, "normal"));
    glEnableVertexAttribArray(glGetAttribLocation(m_shader, "texCoord"));
    glVertexAttribPointer(
                    glGetAttribLocation(m_shader, "position"),
                    3, // Num coords per position
                    GL_FLOAT, // Type of data
                    GL_FALSE, // Normalized?
                    sizeof(GLHVertex_VNT), // Stride between entries
                    (void *)0 // Start location offset in the buffer
                    );
    glVertexAttribPointer(
                    glGetAttribLocation(m_shader, "normal"),
                    3, // Num coords per position
                    GL_FLOAT, // Type of data
                    GL_TRUE, // Normalized?
                    sizeof(GLHVertex_VNT), // Stride between entries
                    (void *)(3 * sizeof(float)) // Start location offset in the buffer
                    );
    glVertexAttribPointer(
                    glGetAttribLocation(m_shader, "texCoord"),
                    2, // Num coords per position
                    GL_FLOAT, // Type of data
                    GL_FALSE, // Normalized?
                    sizeof(GLHVertex_VNT), // Stride between entries
                    (void *)(6 * sizeof(float)) // Start location offset in the buffer
                    );

    // Unbind the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create the Index Buffer Object
    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    // The indicies are unsigned shorts
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_cylinder.TotalIndex * sizeof(unsigned short), m_cylinder.pIndex16Bit, GL_STATIC_DRAW);

    // Load the textures
    std::cout << "Loading Textures" << std::endl;
    glEnable(GL_TEXTURE_2D);
    m_pineTexID = loadTexture(":/textures/pine.jpg");

    // Unbind the vertex array
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    // Make a tree or three
    for(int i = 0; i < 3; i++){
        generateTree();
    }

    // Mark the initilization as done
    m_OpenGLDidInit = true;

    std::cout << "Done Initilizing!" << std::endl;
}


/**
 * @brief View::loadShaders loads the openGL shaders
 * Should only be called once
 */
void View::loadShaders()
{
    // Don't load the shaders twice
    if(m_OpenGLDidInit)
    {
        return;
    }

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
}

/**
 * @brief View::makeCylinder loads the unit cylinder into a buffer and assigns a VAO for it
 * Should be called only once during initilization
 */
void View::makeCylinder()
{
    // Don't load the cylinder twice
    if(m_OpenGLDidInit)
    {
        return;
    }

}

/**
 * @brief View::initCylinder inits the member unit cylinder m_cylinder
 */
void View::initCylinder()
{
    memset(&m_cylinder, 0, sizeof(glhCylinderObjectf2));
    m_cylinder.IsThereATop=true; m_cylinder.IsThereABottom=true;
    m_cylinder.RadiusA=1.0; m_cylinder.RadiusB=1.0; m_cylinder.Height=1.0;
    m_cylinder.Stacks=20; m_cylinder.Slices=20;
    m_cylinder.IndexFormat=GLH_INDEXFORMAT_16BIT;
    m_cylinder.VertexFormat=GLH_VERTEXFORMAT_VNT;
    m_cylinder.TexCoordStyle[0]=1;
    m_cylinder.ScaleFactorS[0]=m_cylinder.ScaleFactorT[0]=1.0;

    glhCreateCylinderf2(&m_cylinder);
}


/**
 * @brief View::generateTree uses the member TreeMaker to make a tree
 */
void View::generateTree()
{
    m_treemaker.reset(1.0f, m_treeBranches, m_treeLeaves);
    m_treemaker.makeTree();
}

void View::paintGL()
{
    // Draw a grey background so we can see unlight objects
    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Implement the demo rendering here

    // Use the shader program
    glUseProgram(m_shader);

    // Set up the lighting
    clearLights();

    // For testing, use a single standard light
    glm::vec4 lightDirection = glm::normalize(glm::vec4(1.f, -1.f, -1.f, 0.f));
    CS123SceneLightData light;
    memset(&light, 0, sizeof(light));
    light.type = LIGHT_DIRECTIONAL;
    light.dir = lightDirection;
    light.color[0] = light.color[1] = light.color[2] = 1;
    light.id = 0;
    setLight(light);

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
    float diffuse[3] = {1.0f, 1.0f, 1.0f};
    float specular[3] = {1.0f, 1.0f, 1.0f};
    float shininess = 64.0f;

    glUniform3fv(m_uniformLocs["ambient_color"], 1, ambient);
    glUniform3fv(m_uniformLocs["diffuse_color"], 1, diffuse);
    glUniform3fv(m_uniformLocs["specular_color"], 1, specular);
    glUniform1f(m_uniformLocs["shininess"], shininess);

    // Set up the texture
    //glEnable(GL_TEXTURE_2D);
    glUniform1i(m_uniformLocs["useTexture"], 1);
    glUniform1i(m_uniformLocs["tex"], 0); // maps with glActiveTexture, so this is GL_TEXTURE0
    glUniform1f(m_uniformLocs["blend"], 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pineTexID);

    // Draw the example triangle to screen
    glBindVertexArray(m_vaoID);

    // TODO: apply a material and use a camera to pass variables to the shader
    // That way we actually draw something
    // Can set color in fragment shader for debugging


    // Draw the cylinder

    // Render the entire cylinder at once
    //glDrawRangeElements(GL_TRIANGLES, m_cylinder.Start_DrawRangeElements, m_cylinder.End_DrawRangeElements,
        //m_cylinder.TotalIndex, GL_UNSIGNED_SHORT, (void *)0 );

        for(size_t i = 0; i < m_treeBranches->size(); i++)
        {
            // Apply the modeling transformation
            glUniformMatrix4fv(
                        m_uniformLocs["m"], // Shader variable
                        1, // Number of matricies
                        GL_FALSE, //
                        glm::value_ptr(m_treeBranches->at(i)) // Pointer to the first element
                    );

            // Draw the cylinder
            glDrawRangeElements(GL_TRIANGLES, m_cylinder.Start_DrawRangeElements, m_cylinder.End_DrawRangeElements,
                    m_cylinder.TotalIndex, GL_UNSIGNED_SHORT, (void *)0 );
        }
/*

        float arr = {0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0,
                    0.0, 0.0,
                    0.0, 1.0, 0.0,
                    0.0, 1.0, 0.0,
                    0.0, 1.0,
                    1.0, 0.0, 0.0,
                    0.0, 1.0, 0.0,
                    1.0, 0.0,
                    0.0, 1.0, 0.0,
                    0.0, 1.0, 0.0,
                    0.0, 1.0,
                    1.0, 0.0, 0.0,
                    0.0, 1.0, 0.0,
                    1.0, 0.0,
                    1.0, 1.0, 0.0,
                    0.0, 1.0, 0.0,
                    1.0, 1.0};

        for(size_t i = 0; i < m_treeLeaves->size(); i++)
        {
            // Apply the modeling transformation
            glUniformMatrix4fv(
                        m_uniformLocs["m"],
                        1,
                        GL_FALSE,
                        glm::value_ptr(m_treeLeaves->at(i))
                        );



            // Draw the square
            glDrawRangeElements(GL_TRIANGLES,
                                0,
                                48,
                                2,
                                GL_UNSIGNED_INT,



                                (void *)0 );
        }
*/
    glBindVertexArray(0);

    // Unbind the shader
    glUseProgram(m_shader);
}

/**
 * @brief View::clearLights clears the lights in the shader
 * Totally not lifted from OpenGLScene.cpp in the projects
 */
void View::clearLights()
{
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        glUniform3f(glGetUniformLocation(m_shader, ("lightColors" + indexString).c_str()), 0, 0, 0);
    }
}

/**
 * @brief View::setLight sets the passed light in the shader
 * @param light
 */
void View::setLight(const CS123SceneLightData &light)
{
    std::ostringstream os;
    os << light.id;
    std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.

    bool ignoreLight = false;

    GLint lightType;
    switch(light.type)
    {
    case LIGHT_POINT:
        lightType = 0;
        glUniform3fv(glGetUniformLocation(m_shader, ("lightPositions" + indexString).c_str()), 1,
                glm::value_ptr(light.pos));
        break;
    case LIGHT_DIRECTIONAL:
        lightType = 1;
        glUniform3fv(glGetUniformLocation(m_shader, ("lightDirections" + indexString).c_str()), 1,
                glm::value_ptr(glm::normalize(light.dir)));
        break;
    default:
        ignoreLight = true; // Light type not supported
        break;
    }

    float color[3];
    color[0] = light.color[0];
    color[1] = light.color[1];
    color[2] = light.color[2];
    // Set the light to black if we're ignoring it
    if (ignoreLight)
    {
        color[0] = color[1] = color[2] = 0.0f;
    }

    glUniform1i(glGetUniformLocation(m_shader, ("lightTypes" + indexString).c_str()), lightType);
    glUniform3fv(glGetUniformLocation(m_shader, ("lightColors" + indexString).c_str()),
                1, color);
    glUniform3f(glGetUniformLocation(m_shader, ("lightAttenuations" + indexString).c_str()),
            light.function.x, light.function.y, light.function.z);
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

/**
 * @brief View::loadTexture Loads the given texture for use in OpenGL
 * @param filename the path to the texture
 * @return the openGL texture ID for the loaded texture
 */
GLuint View::loadTexture(std::string filename)
{
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
    QImage texture = QGLWidget::convertToGLFormat(image);

    // Generate a new OpenGL texture ID to put our image into
    GLuint id = 0;
    glGenTextures(1, &id);

    // Make the texture we just created the new active texture
    glBindTexture(GL_TEXTURE_2D, id);

    // Copy the image data into the OpenGL texture
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

    // Set filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set coordinate wrapping options
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Finished loading texture " << filename << std::endl;

    // Return the id
    return id;
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
