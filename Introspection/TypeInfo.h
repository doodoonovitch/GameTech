#ifndef INTROSPECTION_TYPEINFO_H
#define INTROSPECTION_TYPEINFO_H

#pragma once

namespace  Introspection
{


class MemberInfo;

class TypeInfo
{
public:

	typedef std::map<std::string, const MemberInfo *> MemberMap;


public:

	TypeInfo();

	~TypeInfo();

	void Init(const std::string& string, size_t size);

	const std::string& GetName(void) const { return _name; }

	size_t GetSize(void) const { return _size; }

	void AddMember(const MemberInfo * member);

	const MemberInfo * GetMember(const std::string& name);

	const MemberMap& GetMembers() const { return _members; }


private:


	std::string _name;
	size_t _size;
	MemberMap _members;
};



} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFO_H