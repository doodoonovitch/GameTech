#ifndef INTROSPECTION_INTROSPECTION_H
#define INTROSPECTION_INTROSPECTION_H

#pragma once

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <iostream>
#include <ostream>
#include <sstream>
#include <cassert>
#include <limits>
#include <codecvt>
#include <cwchar>

#include "tinyxml2\tinyxml2.h"

#include "Macros.h"
#include "RemoveQualifier.h"
#include "ISerializer.h"
#include "IDeserializer.h"
#include "BasicTypeConverters.h"
#include "TypeList.h"
#include "MemberInfo.h"
#include "TypeInfo.h"
#include "TypeInfoTraits.h"
#include "ObjectBase.h"
#include "TypeInfoManager.h"
#include "TextSerializer.h"
#include "TextDeserializer.h"
#include "XMLSerializer.h"
#include "XMLDeserializer.h"

namespace Introspection
{
	extern void InitializeBasicTypes();
}

#endif // INTROSPECTION_INTROSPECTION_H