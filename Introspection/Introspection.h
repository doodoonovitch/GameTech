#ifndef INTROSPECTION_INTROSPECTION_H
#define INTROSPECTION_INTROSPECTION_H

#pragma once


#define PASTE( _, __ )  _##__
#define GENERATE_LINE( _ ) PASTE( GENERATED_TOKEN_, _ )
#define GENERATE_FILE( _ ) PASTE( __FILE__, _ )
#define NAME_GENERATOR( ) GENERATE_FILE( __LINE__ )

//Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type> NAME_GENERATOR( )( #TYPE, sizeof( TYPE ) ); \

#define DEFINE_TYPEINFO( TYPE ) \
	Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type> TypeInfoCreator##TYPE( #TYPE, sizeof( TYPE ) ); \
	Introspection::RemoveQualifier<TYPE>::type *TYPE::NullCast( void ) \
	{ \
		return reinterpret_cast<Introspection::RemoveQualifier<TYPE>::type *>(NULL); \
	} \
	void TYPE::AddMember( const std::string& name, size_t offset, Introspection::TypeInfo *typeInfo, bool serializable ) \
	{ \
		return Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::AddMember( name, offset, typeInfo, serializable ); \
	} \
	void Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::RegisterMembers( void ) \
	{ \
		TYPE::RegisterMembers( ); \
	} \
	void TYPE::RegisterMembers( void )

#define DECLARE_TYPEINFO( TYPE ) \
	static void AddMember( const std::string& name, size_t offset, Introspection::TypeInfo *typeInfo, bool serializable ); \
	static Introspection::RemoveQualifier<TYPE>::type *NullCast( void ); \
	static void RegisterMembers( void )

#define DEFINE_TYPEINFO_POD( TYPE ) \
	Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type> NAME_GENERATOR( )( #TYPE, sizeof( TYPE ) ); \
	void Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::RegisterMembers( void ) \
	{ \
		Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::SetSerialization( Introspection::TextSerializatonPrim<Introspection::RemoveQualifier<TYPE>::type>::GetInstance() ); \
	}

#define ADD_MEMBER( MEMBER, SERIALIZABLE ) \
	AddMember( #MEMBER, (size_t)(&(NullCast( )->MEMBER)), TYPEINFO_OBJECT( NullCast( )->MEMBER ), SERIALIZABLE)

#define SET_SERIALIZATION( TYPE, SERIALIZATION ) \
	Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::SetSerialization( SERIALIZATION )

//
// TYPEINFO_TYPE
// Purpose: Retrieves the proper TypeInfo instance of an object by type.
//
#define TYPEINFO_TYPE( TYPE ) (Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::GetTypeInfo( ))

//
// TYPEINFO_OBJECT
// Purpose: Retrieves the proper TypeInfo instance of an object by an object's type.
//
#define TYPEINFO_OBJECT( OBJECT ) (Introspection::TypeInfoCreator<Introspection::RemoveQualifier<decltype( OBJECT )>::type>::GetTypeInfo( ))

//
// TYPEINFO_STR
// Purpose : Finds a TypeInfo instance by string name
//
#define TYPEINFO_STR( STRING ) (Introspection::TypeInfoManager::GetTypeInfo( STRING ))

//
// TYPEOF
// Purpose : Get the type of an object
//
#define TYPEOF(OBJECT) Introspection::TypeInfoCreator<Introspection::RemoveQualifier<decltype( OBJECT )>::type>::type


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
		GetTypeInfo()->Init(name, size);
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