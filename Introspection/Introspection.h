#ifndef INTROSPECTION_INTROSPECTION_H
#define INTROSPECTION_INTROSPECTION_H

#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <ostream>
#include <sstream>
#include <cassert>
#include <limits>
#include <codecvt>


#include "Macros.h"
#include "RemoveQualifier.h"
#include "ISerializer.h"
#include "TypeList.h"
#include "TypeInfoTraits.h"
#include "TypeInfo.h"
#include "MemberInfo.h"
#include "ObjectBase.h"
#include "TypeInfoManager.h"
#include "TextSerializer.h"

namespace Introspection
{
	extern void InitializeBasicTypes();
}

#endif // INTROSPECTION_INTROSPECTION_H