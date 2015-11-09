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

	typedef TypeInfoTraits<ObjectBase> SelfTypeInfoTraits;
	typedef TypeAndId<ObjectBase, TL_Length<BasicTypeList>::value> TypeNameAndId;
	typedef TYPELIST_1(ObjectBase) ClassHierarchy;
	typedef NullType ParentClass;
	static TypeInfoTraits<ObjectBase> _typeInfoTraits;

	virtual const TypeInfo* GetTypeInfo() const;
	virtual uint32_t GetTypeId() const;
	virtual bool Serialize(ISerializer& serializer) const;

	static ObjectBase* NullCast(void);
	static void RegisterMembers(void);
	
public:

	bool IsKindOf(const ObjectBase* parent) const;
	bool SerializeMember(ISerializer& serializer, const MemberInfo* mi) const;

};

} // namespace  Introspection
#endif // INTROSPECTION_BASEOBJECT_H