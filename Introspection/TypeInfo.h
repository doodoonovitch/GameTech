#ifndef INTROSPECTION_TYPEINFO_H
#define INTROSPECTION_TYPEINFO_H

#pragma once

namespace  Introspection
{


class MemberInfo;
class ISerialization;

class TypeInfo
{
public:

	typedef std::map<std::string, MemberInfo *> MemberMap;
	typedef std::vector<MemberInfo*> MemberList;


public:

	TypeInfo();

	~TypeInfo();

	void Init(const std::string& string, size_t size, bool isPointer);

	const std::string& GetName(void) const { return _name; }

	size_t GetSize(void) const { return _size; }

	bool GetIsPointer() const { return _isPointer; }

	void AddMember(MemberInfo * member);

	const MemberInfo * GetMember(const std::string& name);

	const MemberList& GetMembers() const { return _members; }
	const MemberMap& GetMemberMap() const { return _membersByName; }

	void SetSerialization(const ISerialization* serialization) { _pSerialization = serialization; }
	const ISerialization* GetSerialization() const { return _pSerialization; }


private:

	const ISerialization* _pSerialization;
	std::string _name;
	size_t _size;
	MemberList _members;
	MemberMap _membersByName;
	bool _isPointer;
};



} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFO_H