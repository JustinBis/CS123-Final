#ifndef COMMON_H
#define COMMON_H

/**
  * Common headers used across the program
  */

#include "GL/glew.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <QMessageBox>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// glu.h in different location on macs
#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

#endif // COMMON_H
