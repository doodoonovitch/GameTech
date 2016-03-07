// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés

// TODO: faites référence ici aux en-têtes supplémentaires nécessaires au programme
#pragma warning ( disable:4505 )

#include <GL/glew.h>
//#include <GL/freeglut.h>


#pragma warning( push )
#pragma warning ( disable:4201 4310 )

#define _USE_MATH_DEFINES
#include <cmath>


#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <ktx.h>

#pragma warning(pop)

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <iostream>
#include <ostream>
#include <sstream>
#include <cassert>
#include <codecvt>
#include <cwchar>
#include <iterator>
#include <functional>
#include <algorithm>

#include "Introspection.h"

