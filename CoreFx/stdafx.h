// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclure les en-t�tes Windows rarement utilis�s

// TODO: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
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

