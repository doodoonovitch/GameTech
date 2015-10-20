#ifndef INTROSPECTION_INTROSPECTION_H
#define INTROSPECTION_INTROSPECTION_H

#pragma once


#include "Macros.h"
#include "RemoveQualifier.h"
#include "TypeInfo.h"
#include "MemberInfo.h"
#include "TypeInfoManager.h"
#include "ISerialization.h"
#include "TextSerializationPrim.h"
#include "TextSerialization.h"


namespace Introspection
{

template <typename T>
class TypeInfoCreator
{
public:
	typedef T type;

	TypeInfoCreator(const std::string& name, size_t size)
	{
		Init(name, size);
	}

	static void Init(const std::string& name, size_t size)
	{
		GetTypeInfo()->Init(name, size, std::is_pointer<T>::value);
		RegisterMembers();
	}

	static void AddMember(std::string memberName, size_t memberOffset, TypeInfo *typeinfo, bool serializable)
	{
		GetTypeInfo()->AddMember(new MemberInfo(memberName, memberOffset, typeinfo, serializable));
	}

	static void SetSerialization(const ISerialization* serialization)
	{
		GetTypeInfo()->SetSerialization(serialization);
	}

	static const ISerialization* GetSerialization()  
	{
		GetTypeInfo()->GetSerialization();
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
};


} // namespace  Introspection
#endif // INTROSPECTION_INTROSPECTION_H