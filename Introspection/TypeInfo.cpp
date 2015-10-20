#include "stdafx.h"
#include "Introspection.h"


namespace Introspection
{


TypeInfo::TypeInfo()	
	: _pSerialization(nullptr)
	, _size(0)

{
}


TypeInfo::~TypeInfo()
{
	_pSerialization = nullptr;

	for (auto p : _members)
	{
		delete p;
	}
	_members.clear();
	_membersByName.clear();
}

void TypeInfo::Init(const std::string& name, size_t size, bool isPointer)
{
	_name = name;
	_size = size;
	_isPointer = isPointer;
	TypeInfoManager::Add(this);
}

void TypeInfo::AddMember(MemberInfo *member)
{
	assert(_membersByName.find(member->GetName()) == _membersByName.end());

	_members.push_back(member);
	_membersByName[member->GetName()] = member;
}

const MemberInfo * TypeInfo::GetMember(const std::string& name)
{
	auto it = _membersByName.find(name);
	return it != _membersByName.end() ? it->second : nullptr;
}




} // namespace  Introspection