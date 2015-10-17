#include "stdafx.h"
#include "Introspection.h"


namespace  Introspection
{


TypeInfo::TypeInfo()	
	: _size(0)
{
}


TypeInfo::~TypeInfo()
{
	for (auto p : _members)
	{
		delete p.second;
	}
	_members.clear();
}

void TypeInfo::Init(const std::string& name, size_t size)
{
	_name = name;
	_size = size;
	TypeInfoManager::Add(this);
}

void TypeInfo::AddMember(const MemberInfo *member)
{
	assert(_members.find(member->GetName()) == _members.end());

	_members[member->GetName()] = member;
}

const MemberInfo * TypeInfo::GetMember(const std::string& name)
{
	auto it = _members.find(name);
	return it != _members.end() ? it->second : nullptr;
}


} // namespace  Introspection