#include "stdafx.h"
#include "Introspection.h"


namespace Introspection
{


TypeInfo::TypeInfo()	
	: _parent(nullptr)
	, _serialize(nullptr)
	, _create(nullptr)
	, _typeId(-1)
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

void TypeInfo::Init(const TypeInfo* parent, TypeInfo::SerializeFunc serialize, CreateFunc createFunc, uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size, bool isBasicType)
{
	_parent = parent;
	_serialize = serialize;
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
	assert(_membersByName.find(member->GetName()) == _membersByName.end());

	_members.push_back(member);
	_membersByName[member->GetName()] = member;
}

const MemberInfo* TypeInfo::GetMember(const std::string& name)
{
	auto it = _membersByName.find(name);
	return it != _membersByName.end() ? it->second : nullptr;
}


bool TypeInfo::Serialize(ISerializer* serializer, uintptr_t dataPtr, TypeQualifier typeQualifier) const
{
	assert(serializer != nullptr);
	assert(dataPtr != (uintptr_t)nullptr);
	assert(_serialize != nullptr);

	return _serialize(*serializer, (uintptr_t)dataPtr, typeQualifier);
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
#include "DefineBasicTypeInfo.inl"
}

} // namespace  Introspection