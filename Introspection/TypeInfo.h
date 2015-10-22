#ifndef INTROSPECTION_TYPEINFO_H
#define INTROSPECTION_TYPEINFO_H

#pragma once

namespace  Introspection
{


class TypeInfo
{
public:

	typedef std::map<std::string, MemberInfo *> MemberMap;
	typedef std::vector<MemberInfo*> MemberList;
	typedef bool (*SerializeFunc)(ISerializer& serializer, const void* value);
public:

	const std::string& GetName(void) const { return _name; }
	const std::wstring& GetWName(void) const { return _wname; }

	size_t GetSize(void) const { return _size; }

	bool GetIsBasicType() const { return _isBasicType; }
	bool GetIsPointer() const { return _isPointer; }

	const MemberInfo * GetMember(const std::string& name);

	const MemberList& GetMembers() const { return _members; }
	const MemberMap& GetMemberMap() const { return _membersByName; }

	void AddMember(MemberInfo * member);
	void Init(const std::string& name, const std::wstring& wname, size_t size, bool isBasicType, bool isPointer, SerializeFunc serialize);

	bool Serialize(ISerializer& serializer, const void* value) const;

	TypeInfo();
	virtual ~TypeInfo();

private:

	SerializeFunc _serialize;
	std::string _name;
	std::wstring _wname;
	size_t _size;
	MemberList _members;
	MemberMap _membersByName;
	bool _isBasicType;
	bool _isPointer;
};



template <typename T>
class TypeInfoCreator
{
public:
	typedef T type;

	static void AddMember(const std::string& memberName, const std::wstring& wMemberName, size_t memberOffset, TypeInfo *typeinfo, bool serializable)
	{
		GetTypeInfo()->AddMember(new MemberInfo(memberName, wMemberName, memberOffset, typeinfo, serializable));
	}

	static T *NullCast(void)
	{
		return reinterpret_cast<T *>(nullptr);
	}

	template<typename TObjectType>
	static T* GetPtr(TObjectType* pObject, const MemberInfo* mi)
	{
		return mi->GetPtr(pObject);
	}

	static void RegisterMembers(void);

	// Ensure a single instance can exist for this class type
	static TypeInfo *GetTypeInfo(void)
	{
		static TypeInfo instance;
		return &instance;
	}

private:

	static bool SerializeObject(ISerializer& serializer, const void* value)
	{
		return serializer.SerializeObject(GetTypeInfo(), value);
	}

protected:

	TypeInfoCreator() {}

};

template <typename T>
class ObjectTypeInfoCreator : public TypeInfoCreator<T>
{
public:
	typedef T type;

	ObjectTypeInfoCreator(const std::string& name, const std::wstring& wname, size_t size)
	{
		GetTypeInfo()->Init(name, wname, size, false, std::is_pointer<T>::value, SerializeObject);
		RegisterMembers();
	}

private:

	static bool SerializeObject(ISerializer& serializer, const void* value)
	{
		return serializer.SerializeObject(GetTypeInfo(), value);
	}

	ObjectTypeInfoCreator() = delete;

};


template <typename T>
class BasicTypeInfoCreator : public TypeInfoCreator<T>
{
public:

	BasicTypeInfoCreator(const std::string& name, const std::wstring& wname, size_t size)
	{
		GetTypeInfo()->Init(name, wname, size, true, std::is_pointer<T>::value, SerializeBasicType);
		RegisterMembers();
	}

private:

	static bool SerializeBasicType(ISerializer& serializer, const void* value)
	{
		return serializer.SerializeBasicType(*reinterpret_cast<const T*>(value));
	}

	BasicTypeInfoCreator() = delete;
};

} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFO_H