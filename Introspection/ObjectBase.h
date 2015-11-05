#ifndef INTROSPECTION_BASEOBJECT_H
#define INTROSPECTION_BASEOBJECT_H

#pragma once

namespace  Introspection
{



class ObjectBase
{
	template<typename T>
	friend class TypeInfoTraits;

protected:
	ObjectBase();
	virtual ~ObjectBase();

public:

	typedef TypeAndId<ObjectBase, TL_Length<BasicTypeList>::value> TypeNameAndId;
	typedef TypeInfoTraits<ObjectBase> SelfTypeTraits;

	//static TypeInfoTraits<ObjectBase> _typeInfoTraits(nullptr, TypeNameAndId::TypeId, "BaseObject");

	static void AddMember(const std::string& name, const std::wstring& wname, uintptr_t offset, const Introspection::TypeInfo* typeInfo, bool isPointer, bool serializable);
	static Introspection::RemoveQualifier<ObjectBase>::type* NullCast(void);
	static void RegisterMembers(void);

	virtual const TypeInfo* GetTypeInfo() const;
	virtual uint32_t GetTypeId() const;
	bool IsKindOf(const ObjectBase* parent) const;
	virtual bool Serialize(ISerializer& serializer) const;

	bool SerializeMember(ISerializer& serializer, const MemberInfo* mi) const;

public:

	typedef Introspection::NullType ParentClass;
	typedef TYPELIST_1(ObjectBase) ClassHierarchy;
};

} // namespace  Introspection
#endif // INTROSPECTION_BASEOBJECT_H