#ifndef INTROSPECTION_TYPEINFOCREATOR_H
#define INTROSPECTION_TYPEINFOCREATOR_H

#pragma once

namespace  Introspection
{


class TypeInfo;
class MemberInfo;


template <typename T>
class TypeInfoCreator
{
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

public:
	typedef T type;

	static const bool _isBasicType = TL_Contains<BasicTypeList, T>::value;
	static const bool _isPointer = std::is_pointer<T>::value;

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
		return Select<_isBasicType, SerializeBasictType, SerializeObject>::Result::Serialize(serializer, data);
	}

	TypeInfoCreator(uint32_t typeId, const std::string& name, const std::wstring& wname, size_t size)
	{
		GetTypeInfo()->Init(SerializeMember, typeId, name, wname, size, _isBasicType, _isPointer);
		RegisterMembers();
	}

};


} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFOCREATOR_H