#ifndef INTROSPECTION_INTROSPECTION_H
#define INTROSPECTION_INTROSPECTION_H

#pragma once

#include "RemoveQualifier.h"
#include "TypeInfo.h"
#include "MemberInfo.h"
#include "TypeInfoManager.h"



#define PASTE( _, __ )  _##__
#define GENERATE_LINE( _ ) PASTE( GENERATED_TOKEN_, _ )
#define GENERATE_FILE( _ ) PASTE( __FILE__, _ )
#define NAME_GENERATOR( ) GENERATE_FILE( __LINE__ )


#define DEFINE_TYPEINFO( TYPE ) \
  Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type> NAME_GENERATOR( )( #TYPE, sizeof( TYPE ) ); \
  Introspection::RemoveQualifier<TYPE>::type *TYPE::NullCast( void ) { return reinterpret_cast<Introspection::RemoveQualifier<TYPE>::type *>(NULL); } \
  void TYPE::AddMember( const std::string& name, size_t offset, Introspection::TypeInfo *data ) { return Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::AddMember( name, offset, data ); } \
  void Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::RegisterMembers( void ) { TYPE::RegisterMembers( ); } \
  void TYPE::RegisterMembers( void )

#define DECLARE_TYPEINFO( TYPE ) \
  static void AddMember( const std::string& name, size_t offset, Introspection::TypeInfo *data ); \
  static Introspection::RemoveQualifier<TYPE>::type *NullCast( void ); \
  static void RegisterMembers( void )

#define DEFINE_TYPEINFO_POD( TYPE ) \
  Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type> NAME_GENERATOR( )( #TYPE, sizeof( TYPE ) ); \
  void Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::RegisterMembers( void ) \
  { \
  }

#define ADD_MEMBER( MEMBER ) \
  AddMember( #MEMBER, (size_t)(&(NullCast( )->MEMBER)), TYPEINFO_OBJECT( NullCast( )->MEMBER ))


//
// TYPEINFO_TYPE
// Purpose: Retrieves the proper TypeInfo instance of an object by type.
//
#define TYPEINFO_TYPE( TYPE ) (Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::Get( ))

//
// TYPEINFO_OBJECT
// Purpose: Retrieves the proper TypeInfo instance of an object by an object's type.
//
#define TYPEINFO_OBJECT( OBJECT ) (Introspection::TypeInfoCreator<Introspection::RemoveQualifier<decltype( OBJECT )>::type>::Get( ))

//
// TYPEINFO_STR
// Purpose : Finds a TypeInfo instance by string name
//
#define TYPEINFO_STR( STRING ) (Introspection::TypeInfoManager::Get( STRING ))



namespace Introspection
{

template <typename T>
class TypeInfoCreator
{
public:
	TypeInfoCreator(const std::string& name, size_t size)
	{
		Init(name, size);
	}

	static void Init(const std::string& name, size_t size)
	{
		Get()->Init(name, size);
		RegisterMembers();
	}

	static void AddMember(std::string memberName, size_t memberOffset, TypeInfo *typeinfo)
	{
		Get()->AddMember(new MemberInfo(memberName, memberOffset, typeinfo));
	}

	static T *NullCast(void)
	{
		return reinterpret_cast<T *>(nullptr);
	}

	static void RegisterMembers(void);

	// Ensure a single instance can exist for this class type
	static TypeInfo *Get(void)
	{
		static TypeInfo instance;
		return &instance;
	}
};


} // namespace  Introspection
#endif // INTROSPECTION_INTROSPECTION_H