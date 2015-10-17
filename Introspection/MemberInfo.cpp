#include "stdafx.h"
#include "Introspection.h"



namespace Introspection
{


MemberInfo::MemberInfo(const std::string& name, size_t offset, const TypeInfo* typeInfo)
	: _name(name)
	, _offset(offset)
	, _pType(typeInfo)
{
}


MemberInfo::~MemberInfo()
{
}


} // namespace  Introspection
