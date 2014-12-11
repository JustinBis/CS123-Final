QT += core gui opengl

TARGET = final
TEMPLATE = app

# If you add your own folders, add them to INCLUDEPATH and DEPENDPATH, e.g.
INCLUDEPATH += glm lib
DEPENDPATH += glm lib

SOURCES += main.cpp \
    mainwindow.cpp \
    view.cpp \
    lib/ResourceLoader.cpp \
    camera.cpp

HEADERS += mainwindow.h \
    view.h \
    lib/ResourceLoader.h \
    camera.h \
    Common.h

FORMS += mainwindow.ui


# For local development
#QMAKE_CXXFLAGS += -std=c++11 # Why does this break glm?
#QMAKE_CXXFLAGS += -stdlib=libc++ # Use Clang's c++11 library # Or don't, thanks Apple
QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64

#LIBS += -L/course/cs123/lib/glew/glew-1.10.0/include -lGLEW
LIBS += -L/usr/local/Cellar/glew/1.11.0/lib -lGLEW

#INCLUDEPATH += /course/cs123/lib/glew/glew-1.10.0/include
#DEPENDPATH += /course/cs123/lib/glew/glew-1.10.0/include
INCLUDEPATH+=/usr/local/Cellar/glew/1.11.0/include
DEPENDPATH+=/usr/local/Cellar/glew/1.11.0/include

OTHER_FILES += \
    shaders/shader.frag \
    shaders/shader.vert

RESOURCES += \
    resources.qrc
