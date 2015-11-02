#include "stdafx.h"
#include "Introspection.h"


namespace Introspection
{



TypeInfoManager::TypeInfoManager()
{
}

TypeInfoManager::~TypeInfoManager()
{
	_typeInfoByName.clear();
}

const TypeInfo* TypeInfoManager::GetTypeInfoInstance(const std::string& name) const
{
	auto it = _typeInfoByName.find(name);
	return it == _typeInfoByName.end() ? nullptr : it->second;
}

const TypeInfo* TypeInfoManager::GetTypeInfoInstance(const std::wstring& name) const
{
	auto it = _typeInfoByWName.find(name);
	return it == _typeInfoByWName.end() ? nullptr : it->second;
}

void TypeInfoManager::AddTypeInfo(const TypeInfo* typeInfo)
{
	assert(typeInfo != nullptr);

	const std::string& typeName = typeInfo->GetTypeName();

	assert(_typeInfoByName.find(typeName) == _typeInfoByName.end());

	_typeInfoByName[typeName] = typeInfo;
	_typeInfoByWName[typeInfo->GetTypeWName()] = typeInfo;
}




} // namespace Introspection