#include "stdafx.h"
#include "Introspection.h"


namespace Introspection
{


TypeInfo::TypeInfo()	
	: _parent(nullptr)
	, _serialize(nullptr)
	, _create(nullptr)
	, _typeId((uint32_t)-1)
	, _size(0)
	, _isBasicType(false)
{
}

TypeInfo::~TypeInfo()
{
	for (auto p : _members)
	{
		delete p;
	}
	_members.clear();
	_membersByName.clear();
}

void TypeInfo::Init(const TypeInfo* parent, TypeInfo::SerializeFunc serialize, DeserializeFunc deserialize, CreateFunc createFunc, uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size, bool isBasicType)
{
	_parent = parent;
	_serialize = serialize;
	_deserialize = deserialize;
	_create = createFunc;
	_typeId = typeId;
	_name = name;
	_wname = wname;
	_size = size;
	_isBasicType = isBasicType;
	TypeInfoManager::Add(this);
}

void TypeInfo::AddMember(MemberInfo *member)
{
	_members.push_back(member);
	_membersByName[member->GetName()] = member;
	_membersByWName[member->GetWName()] = member;
}

const MemberInfo* TypeInfo::GetMember(const std::string& name, bool includeInherit) const
{
	auto it = _membersByName.find(name);

	if (it != _membersByName.end())
		return it->second;

	if (includeInherit)
	{
		if (_parent != nullptr)
			return _parent->GetMember(name, true);
	}

	return nullptr;
}


const MemberInfo* TypeInfo::GetMember(const std::wstring& name, bool includeInherit) const
{
	auto it = _membersByWName.find(name);

	if (it != _membersByWName.end())
		return it->second;

	if (includeInherit)
	{
		if (_parent != nullptr)
			return _parent->GetMember(name, true);
	}

	return nullptr;
}



bool TypeInfo::IsKindOf(uint32_t typeId) const
{
	if (_typeId == typeId)
	{
		return true;
	}

	if (_parent != nullptr)
	{
		return _parent->IsKindOf(typeId);
	}

	return false;

}

void TypeInfo::Create(void*& var) const
{
	if (_create == nullptr)
		var = nullptr;
	else
		_create(var);
}


void InitializeBasicTypes()
{
	static bool isInitialized = false;
	if (isInitialized)
		return;

	isInitialized = true;

#include "DefineBasicTypeInfo.inl"
}

} // namespace  Introspection