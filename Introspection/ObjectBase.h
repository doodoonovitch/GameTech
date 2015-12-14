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
	virtual void Serialize(ISerializer& serializer) const;

	static ObjectBase* NullCast(void);
	static void RegisterMembers(void);
	
public:

	bool IsKindOf(const ObjectBase* parent) const;
	void SerializeMember(ISerializer& serializer, const MemberInfo* mi) const;
	void DeserializeMember(IDeserializer& serializer, const MemberInfo* mi) const;

protected:

	void AddParentMembers(const TypeInfo* /*parent*/)
	{
	}

	template<typename TClassHierarchy> struct ParseParentMembers;

	template<>	struct ParseParentMembers<NullType>
	{
		static void Parse() {}
	};

	template<typename TClassHierarch> struct ParseParentMembers
	{
	private:
		ASSERT_TYPELIST(TClassHierarch);

		typedef typename TClassHierarch::Head Head;
		typedef typename TClassHierarch::Tail Tail;

	public:
		static void Parse()
		{
			ParseParentMembers<Tail>::Parse();
		}
	};


};

} // namespace  Introspection
#endif // INTROSPECTION_BASEOBJECT_H