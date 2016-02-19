// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés



// TODO: faites référence ici aux en-têtes supplémentaires nécessaires au programme
#pragma warning ( disable:4505 )

#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/glew.h>
#include <GL/freeglut.h>


#pragma warning( push )
#pragma warning ( disable:4201 4310 )

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <SOIL.h>

#pragma warning(pop)

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <iterator>
#include <functional>

//#define _USE_MATH_DEFINES
//#include <cmath>
//#undef _USE_MATH_DEFINES

#include <algorithm>

#include "Introspection.h"

