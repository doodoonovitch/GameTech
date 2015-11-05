#include "stdafx.h"
#include "Introspection.h"



namespace Introspection
{


MemberInfo::MemberInfo(const std::string& name, const std::wstring& wname, uintptr_t offset, const TypeInfo* typeInfo, bool isPointer, bool serializable)
	: _name(name)
	, _wname(wname)
	, _offset(offset)
	, _pType(typeInfo)
	, _isPointer(isPointer)
	, _serializable(serializable)
{
}


MemberInfo::~MemberInfo()
{
}


} // namespace  Introspection
