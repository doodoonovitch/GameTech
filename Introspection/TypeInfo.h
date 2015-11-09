#ifndef INTROSPECTION_TYPEINFO_H
#define INTROSPECTION_TYPEINFO_H

#pragma once

namespace  Introspection
{
	
	class MemberInfo;


class TypeInfo
{
	template<typename T>
	friend class TypeInfoTraits;

public:

	typedef std::map<std::string, MemberInfo*> MemberMap;
	typedef std::vector<MemberInfo*> MemberList;
	typedef bool (*SerializeFunc)(ISerializer& serializer, uintptr_t varptr, bool isPointer, uint32_t extent);
	typedef void(*CreateFunc)(void*& var);

public:

	const std::string& GetTypeName(void) const { return _name; }
	const std::wstring& GetTypeWName(void) const { return _wname; }

	uint32_t GetTypeId() const
	{
		return _typeId;
	}

	size_t GetTypeSize(void) const { return _size; }

	bool GetIsBasicType() const { return _isBasicType; }

	bool IsRoot() const { return _parent == nullptr; }
	bool IsA(uint32_t typeId) const { return _typeId == typeId; }
	bool IsA(const TypeInfo* typeInfo) const { return _typeId == typeInfo->_typeId; }

	bool IsKindOf(uint32_t typeId) const;
	bool IsKindOf(const TypeInfo* typeInfo) const { return IsKindOf(typeInfo->GetTypeId()); }

	const TypeInfo* GetParent() const { return _parent; }

	const MemberInfo* GetMember(const std::string& name);

	const MemberList& GetMembers() const { return _members; }
	const MemberMap& GetMemberMap() const { return _membersByName; }

	template<typename TMemberType>
	void AddMember(const ::std::string& memberName, const ::std::wstring& wMemberName, uintptr_t memberOffset, bool serializable)
	{
		auto typeinfo = ::Introspection::TypeInfoTraits<::Introspection::GetVarType<TMemberType>::type>::GetTypeInfo();
		bool isPointer = ::Introspection::Is_Var_Pointer<TMemberType>::value;
		uint32_t extent = std::extent<TMemberType>::value;
		::Introspection::MemberInfo* member = new ::Introspection::MemberInfo(memberName, wMemberName, memberOffset, typeinfo, isPointer, extent, serializable);
		AddMember(member);
	}

	bool Serialize(ISerializer* serializer, uintptr_t dataPtr, bool isPointer, uint32_t itemCount) const;

	void Create(void*& var) const;

protected:

	void AddMember(MemberInfo* member);
	void Init(const TypeInfo* parent, SerializeFunc serialize, CreateFunc createFunc, uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size, bool isBasicType);

	TypeInfo();
	virtual ~TypeInfo();

private:

	const TypeInfo* _parent;
	SerializeFunc _serialize;
	CreateFunc _create;
	uint32_t _typeId;
	size_t _size;
	std::string _name;
	std::wstring _wname;
	MemberList _members;
	MemberMap _membersByName;
	bool _isBasicType;
};



} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFO_H