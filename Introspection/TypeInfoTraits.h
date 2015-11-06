#ifndef INTROSPECTION_TYPEINFOCREATOR_H
#define INTROSPECTION_TYPEINFOCREATOR_H

#pragma once

namespace  Introspection
{


class TypeInfo;
class MemberInfo;


enum class TypeQualifier
{
	None,
	Pointer,
	Array
};

template<typename T>
struct Is_Basic_Type
{
	enum
	{
		value = TL_Contains<BasicTypeList, T>::value,
	};
};

template<typename TRaw>
struct GetVarTypeQualifier
{
private:

	typedef typename Introspection::RemoveQualifier<TRaw>::type T;

	typedef std::integral_constant<TypeQualifier, TypeQualifier::None> TypeQualifier_None;
	typedef std::integral_constant<TypeQualifier, TypeQualifier::Pointer> TypeQualifier_Pointer;
	typedef std::integral_constant<TypeQualifier, TypeQualifier::Array> TypeQualifier_Array;
	
	struct Conditional3
	{
		static constexpr TypeQualifier value = std::conditional<std::is_array<T>::value, TypeQualifier_Array, TypeQualifier_None>::type::value;
	};

	struct PointerOrCString
	{
		static constexpr TypeQualifier value = std::conditional<(std::is_same<T,char*>::value || std::is_same<T, wchar_t*>::value), TypeQualifier_None, TypeQualifier_Pointer>::type::value;
	};

	struct Conditional1
	{
		static constexpr TypeQualifier value = std::conditional<std::is_pointer<T>::value, PointerOrCString, Conditional3>::type::value;
	};


public:

	static constexpr TypeQualifier value = Conditional1::value;

};


template<typename TRaw>
struct GetVarType
{
private:

	typedef typename Introspection::RemoveQualifier<TRaw>::type T;

	struct Conditional3
	{
		typedef typename std::conditional<std::is_array<T>::value, typename std::remove_extent<T>::type, T>::type type;
	};

	struct PointerOrCString
	{
		typedef typename std::conditional<(std::is_same<T, char*>::value || std::is_same<T, wchar_t*>::value), T, typename std::remove_pointer<T>::type>::type type;
	};

	struct Conditional1
	{
		typedef typename std::conditional<std::is_pointer<T>::value, typename PointerOrCString::type, typename Conditional3::type>::type type;
	};


public:

	typedef typename Conditional1::type type;
		
};

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
		IsBasicType = Is_Basic_Type<T>::value,
	};

	typedef T SelfClass;
	//typedef typename std::conditional<IsBasicType, BasicTypeHierarchy, ObjectTypeHierarchy<T>>::type::ClassHierarchy ClassHierarchy;

	// T is a kind of U
	//template<typename U>
	//static bool IsKindOf()
	//{
	//	return TL_Contains<ClassHierarchy, U>::value;
	//}

	static void AddMember(const std::string& memberName, const std::wstring& wMemberName, size_t memberOffset, const TypeInfo* typeinfo, TypeQualifier typeQualifier, bool serializable)
	{
		GetTypeInfo()->AddMember(new MemberInfo(memberName, wMemberName, memberOffset, typeinfo, typeQualifier, serializable));
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

	// Ensure a single instance can exist for this class type
	static TypeInfo *GetTypeInfo(void)
	{
		static TypeInfo instance;
		return &instance;
	}

	static bool Serialize(ISerializer& serializer, const T& data)
	{
		return std::conditional<IsBasicType, SerializeBasicType, SerializeObject>::type::Serialize(serializer, data);
	}

	TypeInfoTraits(const TypeInfo* parent, uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size)
	{
		GetTypeInfo()->Init(parent, RTSerialize, CreateMember, typeId, name, wname, size, IsBasicType);
		std::conditional<IsBasicType, BasicTypeNoRegisterMembers, RegisterClassMembers>::type::RegisterMembers();
	}

private:

	struct RegisterClassMembers
	{
		static void RegisterMembers()
		{
			T::RegisterMembers();
		}
	};

	struct BasicTypeNoRegisterMembers
	{
		static void RegisterMembers()
		{
		}
	};

	struct SerializeBasicType
	{
		static bool Serialize(ISerializer& serializer, const T value)
		{
			return serializer.SerializeBasicType(value);
		}
	};

	struct SerializeObjectPointer
	{
		static bool Serialize(ISerializer& serializer, const T& obj)
		{
			return serializer.SerializeObject(*obj);
		}
	};

	struct SerializeObjectValue
	{
		static bool Serialize(ISerializer& serializer, const T& obj)
		{
			return serializer.SerializeObject(obj);
		}
	};

	struct SerializeObject
	{
		static bool Serialize(ISerializer& serializer, const T& obj)
		{
			return serializer.SerializeObject(obj);
		}
	};

	static bool RTSerialize(ISerializer& serializer, uintptr_t value, TypeQualifier typeQualifier)
	{
		switch (typeQualifier)
		{
		case TypeQualifier::Pointer:
		{
			const T* ptr = *(const T**)(value);
			return Serialize(serializer, *ptr);
		}
		break;

		case TypeQualifier::Array:
		{
			return false;
		}
		break;

		case TypeQualifier::None:
		default:
		{
			return Serialize(serializer, *reinterpret_cast<const T*>(value));
		}
		break;
		}
	}

	static void CreateMember(void*& var)
	{
		var = new T();
	}

};


} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFOCREATOR_H