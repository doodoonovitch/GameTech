#ifndef INTROSPECTION_BASEOBJECT_H
#define INTROSPECTION_BASEOBJECT_H

#pragma once

namespace  Introspection
{




class ObjectBase
{
protected:
	ObjectBase();
	virtual ~ObjectBase();

public:

	virtual const TypeInfo* GetTypeInfo() const;
	virtual uint32_t GetTypeId() const;

	static void AddMember(const std::string& name, const std::wstring& wname, uintptr_t offset, Introspection::TypeInfo* typeInfo, bool serializable); 
	static Introspection::RemoveQualifier<ObjectBase>::type* NullCast(void);
	static void RegisterMembers(void);

	virtual bool Serialize(ISerializer& serializer) const = 0;

	bool SerializeMember(ISerializer& serializer, const MemberInfo* mi) const;

public:

	static const uint32_t _typeid = TL_Length<BasicTypeList>::value;

	typedef Introspection::NullType ParentClass;
	//typedef Introspection::Typelist<ObjectBase, Introspection::NullType> ClassHierarchy;
};

} // namespace  Introspection
#endif // INTROSPECTION_BASEOBJECT_H