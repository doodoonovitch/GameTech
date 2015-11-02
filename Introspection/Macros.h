#pragma once

#define PASTE( _, __ )  _##__
#define GENERATE_LINE( _ ) PASTE( GENERATED_TOKEN_, _ )
#define GENERATE_FILE( _ ) PASTE( __FILE__, _ )
#define NAME_GENERATOR( ) GENERATE_FILE( __LINE__ )


#define DECLARE_TYPEINFO( TYPE, PARENT ) \
	virtual const Introspection::TypeInfo* GetTypeInfo() const override; \
	virtual uint32_t GetTypeId() const override; \
	virtual bool Serialize(Introspection::ISerializer& serializer) const override; \
	static void AddMember( const std::string& name, const std::wstring& wname, uintptr_t offset, Introspection::TypeInfo *typeInfo, bool serializable ); \
	static Introspection::RemoveQualifier<TYPE>::type *NullCast( void ); \
	static void RegisterMembers( void )

#define DEFINE_TYPEINFO( TYPE, TYPEID ) \
	const Introspection::TypeInfo* TYPE::GetTypeInfo() const { return Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::GetTypeInfo(); } \
	uint32_t TYPE::GetTypeId() const { return Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::GetTypeInfo()->GetTypeId(); } \
	bool TYPE::Serialize(Introspection::ISerializer& serializer) const \
	{ \
		return Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::Serialize(serializer, *static_cast<const Introspection::RemoveQualifier<TYPE>::type*>(this)); \
	} \
	Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type> NAME_GENERATOR( )( (TYPEID), #TYPE, L#TYPE, sizeof( TYPE ) ); \
	Introspection::RemoveQualifier<TYPE>::type* TYPE::NullCast( void ) \
	{ \
		return reinterpret_cast<Introspection::RemoveQualifier<TYPE>::type *>(NULL); \
	} \
	void TYPE::AddMember( const std::string& name, const std::wstring& wname, uintptr_t offset, Introspection::TypeInfo *typeInfo, bool serializable ) \
	{ \
		return Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::AddMember( name, wname, offset, typeInfo, serializable ); \
	} \
	void Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::RegisterMembers( void ) \
	{ \
		printf("RegisterMembers for %s.\n", #TYPE);\
		TYPE::RegisterMembers( ); \
	} \
	void TYPE::RegisterMembers( void )

#define DEFINE_TYPEINFO_BASICTYPE( TYPE ) \
	Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type> NAME_GENERATOR( )( Introspection::TL_IndexOf<Introspection::BasicTypeList, TYPE>::value, #TYPE, L#TYPE, sizeof( TYPE ) ); \
	void Introspection::TypeInfoCreator<Introspection::RemoveQualifier<TYPE>::type>::RegisterMembers( void ) \
	{ \
		printf("RegisterMembers for %s.\n", #TYPE);\
	}

#define ADD_MEMBER( MEMBER, SERIALIZABLE ) \
	AddMember( #MEMBER, L#MEMBER, (uintptr_t)(&(NullCast( )->MEMBER)), TYPEINFO_OBJECT( NullCast( )->MEMBER ), SERIALIZABLE)

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
#define TYPEOF(OBJECT) (Introspection::TypeInfoCreator<Introspection::RemoveQualifier<decltype( OBJECT )>::type>::type)


