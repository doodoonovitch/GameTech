#ifndef INTROSPECTION_TYPEINFO_H
#define INTROSPECTION_TYPEINFO_H

#pragma once

namespace  Introspection
{


class TypeInfo
{
public:

	typedef std::map<std::string, MemberInfo*> MemberMap;
	typedef std::vector<MemberInfo*> MemberList;
	typedef bool (*SerializeMemberFunc)(ISerializer& serializer, const void* value);

public:

	const std::string& GetName(void) const { return _name; }
	const std::wstring& GetWName(void) const { return _wname; }

	size_t GetSize(void) const { return _size; }

	bool GetIsBasicType() const { return _isBasicType; }
	bool GetIsPointer() const { return _isPointer; }

	const MemberInfo* GetMember(const std::string& name);

	const MemberList& GetMembers() const { return _members; }
	const MemberMap& GetMemberMap() const { return _membersByName; }

	bool Serialize(ISerializer* serializer, const void* data) const;

	void AddMember(MemberInfo* member);
	void Init(SerializeMemberFunc serializeMember, uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size, bool isBasicType, bool isPointer);

	uint32_t GetTypeId() const
	{
		return _typeId;
	}

	TypeInfo();
	virtual ~TypeInfo();

private:

	SerializeMemberFunc _serializeMember;
	uint32_t _typeId;
	size_t _size;
	std::string _name;
	std::wstring _wname;
	MemberList _members;
	MemberMap _membersByName;
	bool _isBasicType;
	bool _isPointer;
};



} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFO_H