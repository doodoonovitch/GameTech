#ifndef INTROSPECTION_TYPEINFOCREATOR_H
#define INTROSPECTION_TYPEINFOCREATOR_H

#pragma once

namespace  Introspection
{


class TypeInfo;
class MemberInfo;


template<typename TType, uint32_t TId>
struct TypeAndId
{
	typedef TType Type;
	enum { TypeId = TId };
};


template <typename T>
class TypeInfoTraits
{

	struct BasicTypeHierarchy
	{
		typedef NullType ClassHierarchy;
	};

	template<typename T>
	struct ObjectTypeHierarchy
	{
		typedef typename T::ClassHierarchy ClassHierarchy;
	};

public:

	enum
	{
		IsBasicType = TL_Contains<BasicTypeList, T>::value,
		IsPointer = std::is_pointer<T>::value
	};

	typedef T SelfClass;
	typedef typename Select<TypeInfoTraits<T>::IsBasicType, BasicTypeHierarchy, ObjectTypeHierarchy<T>>::Result::ClassHierarchy ClassHierarchy;

	// T is a kind of U
	template<typename U>
	static bool IsKindOf()
	{
		return TL_Contains<ClassHierarchy, U>::value;
	}

	static void AddMember(const std::string& memberName, const std::wstring& wMemberName, size_t memberOffset, TypeInfo* typeinfo, bool serializable)
	{
		GetTypeInfo()->AddMember(new MemberInfo(memberName, wMemberName, memberOffset, typeinfo, serializable));
	}

	static T *NullCast(void)
	{
		return reinterpret_cast<T*>(nullptr);
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

	static bool Serialize(ISerializer& serializer, const T& data)
	{
		return Select<IsBasicType, SerializeBasictType, SerializeObject>::Result::Serialize(serializer, data);
	}

	TypeInfoTraits(const TypeInfo* parent, uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size)
	{
		GetTypeInfo()->Init(parent, SerializeMember, CreateMember, typeId, name, wname, size, IsBasicType, IsPointer);
		RegisterMembers();
	}

private:


	struct SerializeBasictType
	{
		static bool Serialize(ISerializer& serializer, const T& value)
		{
			return serializer.SerializeBasicType(value);
		}
	};

	struct SerializeObject
	{
		static bool Serialize(ISerializer& serializer, const T& obj)
		{
			return serializer.SerializeObject(obj);
		}
	};

	static bool SerializeMember(ISerializer& serializer, const void* value)
	{
		return Serialize(serializer, *reinterpret_cast<const T*>(value));
	}

	static void CreateMember(void*& var)
	{
		var = new T();
	}

};


} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFOCREATOR_H