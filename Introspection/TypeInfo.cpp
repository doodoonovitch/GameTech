#include "stdafx.h"
#include "Introspection.h"


namespace Introspection
{


TypeInfo::TypeInfo()	
	: _parent(nullptr)
	, _serializeMember(nullptr)
	, _typeId(-1)
	, _size(0)
	, _isBasicType(false)
	, _isPointer(false)
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

void TypeInfo::Init(const TypeInfo* parent, TypeInfo::SerializeMemberFunc serializeMember, uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size, bool isBasicType, bool isPointer)
{
	//static bool firstTime = true;
	//if (firstTime)
	//{
	//	#include "DefineBasicTypeInfo.inl"
	//	firstTime = false;
	//}
	
	_parent = parent;
	_serializeMember = serializeMember;
	_typeId = typeId;
	_name = name;
	_wname = wname;
	_size = size;
	_isBasicType = isBasicType;
	_isPointer = isPointer;
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


bool TypeInfo::Serialize(ISerializer* serializer, const void* data) const
{
	assert(serializer != nullptr);
	assert(data != nullptr);
	assert(_serializeMember != nullptr);

	return _serializeMember(*serializer, data);
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


} // namespace  Introspection