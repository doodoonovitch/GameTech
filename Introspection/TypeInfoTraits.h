#ifndef INTROSPECTION_TYPEINFOCREATOR_H
#define INTROSPECTION_TYPEINFOCREATOR_H

#pragma once

namespace  Introspection
{




template<typename T>
struct Is_Basic_Type
{
	static constexpr bool value = TL_Contains<BasicTypeList, T>::value;
};


template<typename TRaw>
struct Is_Var_Pointer
{
private:

	typedef typename Introspection::RemoveQualifier<TRaw>::type T;

	template<typename T1>
	struct Is_Array_of_Pointer
	{
	private:
		typedef typename std::remove_extent<T1>::type TNoExtent;
		typedef typename std::remove_cv<TNoExtent>::type TNoExtentNoCV;

	public:
		typedef typename std::conditional<std::is_pointer<TNoExtentNoCV>::value, std::true_type, std::false_type>::type type;
	};

	template<typename T1>
	struct Is_Pointer
	{
	private:
		typedef typename std::remove_cv<T1>::type TNoCV;

	public:
		typedef typename std::is_pointer<TNoCV>::type type;
	};

public:

	static constexpr bool value = std::conditional < (std::rank<T>::value > 0), typename Is_Array_of_Pointer<T>::type, typename Is_Pointer<T>::type > ::type::value;
};


template<typename TRaw>
struct GetVarType
{
private:

	typedef typename RemoveQualifier<TRaw>::type T;

public:

	typedef typename RemoveQualifier< typename std::remove_pointer < typename RemoveQualifier< typename std::remove_extent<T>::type >::type >::type >::type type;
		
};


template<typename TType, uint32_t TId>
struct TypeAndId
{
	typedef TType Type;
	static constexpr uint32_t TypeId = TId;
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

	static constexpr bool IsBasicType = Is_Basic_Type<T>::value;

	typedef T SelfClass;
	typedef typename std::conditional<IsBasicType, BasicTypeHierarchy, ObjectTypeHierarchy<T>>::type::ClassHierarchy ClassHierarchy;

	// T is a kind of U
	template<typename U>
	static bool IsKindOf()
	{
		return TL_Contains<ClassHierarchy, U>::value;
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
		InitializeBasicTypes();
		static TypeInfo instance;
		return &instance;
	}

	static void Serialize(ISerializer& serializer, const T& data)
	{
		return serializer.Serialize(data);
	}

	static void Serialize(ISerializer& serializer, const T* const data)
	{
		return serializer.Serialize(data);
	}

	static void SerializeArray(ISerializer& serializer, T const * const  data, uint32_t itemCount)
	{
		serializer.SerializeArray(data, itemCount);
	}

	static void SerializeArray(ISerializer& serializer, T const * const * const data, uint32_t itemCount)
	{
		std::conditional<std::is_base_of<ObjectBase, T>::value, SerializeObjectBasePointerArray<T>, SerializeBasicTypePointerArray<T>>::type::Serialize(serializer, data, itemCount);
	}

	static void Deserialize(IDeserializer& serializer, T& data)
	{
		return serializer.Deserialize(data);
	}

	static void Deserialize(IDeserializer& serializer, T*& data)
	{
		//return serializer.Deserialize(data);
		std::conditional<std::is_base_of<ObjectBase, T>::value, DeserializeObjectBasePointer<T>, DeserializeBasicTypePointer<T>>::type::Deserialize(serializer, data);
	}

	static void DeserializeArray(IDeserializer& serializer, T * data, uint32_t itemCount)
	{
		serializer.DeserializeArray(data, itemCount);
	}

	static void DeserializeArray(IDeserializer& serializer, T * * data, uint32_t itemCount)
	{
		std::conditional<std::is_base_of<ObjectBase, T>::value, DeserializeObjectBasePointerArray<T>, DeserializeBasicTypePointerArray<T>>::type::Deserialize(serializer, data, itemCount);
	}

	TypeInfoTraits(const TypeInfo* parent, uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size)
	{
		GetTypeInfo()->Init(parent, RTSerialize, RTDeserialize, CreateMember, typeId, name, wname, size, IsBasicType);
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

	template<typename T1>
	struct SerializeBasicTypePointerArray
	{
		static void Serialize(ISerializer& serializer, T1 const * const * const data, uint32_t itemCount)
		{
			serializer.SerializeArray(data, itemCount);
		}
	};

	template<typename T1>
	struct SerializeObjectBasePointerArray
	{
		static void Serialize(ISerializer& serializer, T1 const * const * const data, uint32_t itemCount)
		{
			serializer.SerializeArray((ObjectBase const * const * const)data, itemCount);
		}
	};

	static void RTSerialize(ISerializer& serializer, uintptr_t value, bool isPointer, uint32_t extent)
	{
		if (extent == 0)
		{
			if (isPointer)
			{
				T const * const ptr = *(T const * const * const)(value);
				Serialize(serializer, ptr);
			}
			else
			{
				Serialize(serializer, *reinterpret_cast<const T*>(value));
			}
		}
		else
		{
			if (isPointer)
			{
				T const * const * const ptr = (T const * const * const)(value);
				SerializeArray(serializer, ptr, extent);
			}
			else
			{
				T const * const ptr = (T const * const)(value);
				SerializeArray(serializer, ptr, extent);
			}
		}
	}

	template<typename T1>
	struct DeserializeBasicTypePointer
	{
		static void Deserialize(IDeserializer& serializer, T1 * & data)
		{
			serializer.Deserialize(data);
		}
	};

	template<typename T1>
	struct DeserializeObjectBasePointer
	{
		static void Deserialize(IDeserializer& serializer, T1 *& data)
		{
			serializer.Deserialize((ObjectBase *&)data);
		}
	};

	template<typename T1>
	struct DeserializeBasicTypePointerArray
	{
		static void Deserialize(IDeserializer& serializer, T1 * * data, uint32_t itemCount)
		{
			serializer.DeserializeArray(data, itemCount);
		}
	};

	template<typename T1>
	struct DeserializeObjectBasePointerArray
	{
		static void Deserialize(IDeserializer& serializer, T1 * * data, uint32_t itemCount)
		{
			serializer.DeserializeArray((ObjectBase * *)data, itemCount);
		}
	};

	static void RTDeserialize(IDeserializer& serializer, uintptr_t value, bool isPointer, uint32_t extent)
	{
		if (extent == 0)
		{
			if (isPointer)
			{
				T *& ptr = *(T * *)(value);
				if (ptr == nullptr)
				{
					ptr = new T();
				}
				Deserialize(serializer, ptr);
			}
			else
			{
				Deserialize(serializer, *reinterpret_cast<T *>(value));
			}
		}
		else
		{
			if (isPointer)
			{
				T * * ptr = (T * *)(value);
				for (uint32_t i = 0; i < extent; ++i)
				{
					if (ptr[i] == nullptr)
					{
						ptr[i] = new T();
					}
				}
				DeserializeArray(serializer, ptr, extent);
			}
			else
			{
				T * ptr = (T *)(value);
				DeserializeArray(serializer, ptr, extent);
			}
		}
	}

	static void CreateMember(void*& var)
	{
		var = new T();
	}

};


} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFOCREATOR_H