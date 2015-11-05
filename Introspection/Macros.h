#pragma once

#define PASTE( _, __ )  _##__
#define GENERATE_LINE( _ ) PASTE( GENERATED_TOKEN_, _ )
#define GENERATE_FILE( _ ) PASTE( __FILE__, _ )
#define NAME_GENERATOR( ) GENERATE_FILE( __LINE__ )



//virtual bool IsKindOf(const ObjectBase* parent) const override { return TYPEINFO_TYPE(TYPE)->IsKindOf(parent->GetTypeId()); } 
#define DECLARE_TYPEINFO( TYPE, TYPEID, PARENT ) \
	template<typename T> friend class TypeInfoTraits; \
	typedef ::Introspection::TypeInfoTraits<TYPE> SelfTypeTraits; \
	typedef ::Introspection::TypeAndId<TYPE, (TYPEID)> TypeNameAndId; \
	typedef PARENT ParentClass; \
	typedef ::Introspection::Typelist<TYPE, PARENT::ClassHierarchy> ClassHierarchy; \
	virtual const ::Introspection::TypeInfo* GetTypeInfo() const override; \
	virtual uint32_t GetTypeId() const override; \
	virtual bool Serialize(::Introspection::ISerializer& serializer) const override; \
	static void AddMember( const ::std::string& name, const ::std::wstring& wname, uintptr_t offset, const ::Introspection::TypeInfo *typeInfo, bool isPointer, bool serializable ); \
	static TYPE *NullCast( void ); \
	static void RegisterMembers( void )

#define DEFINE_TYPEINFO( TYPE ) \
	DEFINE_TYPEINFO_2( TYPE, TYPE::TypeNameAndId::TypeId, TYPE::ParentClass::SelfTypeTraits::GetTypeInfo() )

#define DEFINE_TYPEINFO_OBJECTBASE( TYPE ) \
	DEFINE_TYPEINFO_2( TYPE, TYPE::TypeNameAndId::TypeId, nullptr )

#define DEFINE_TYPEINFO_2( TYPE, TYPEID, PARENT_TYPE_INFO ) \
	TYPE::SelfTypeTraits NAME_GENERATOR( )( PARENT_TYPE_INFO, (TYPEID), #TYPE, L#TYPE, sizeof( TYPE ) ); \
	const ::Introspection::TypeInfo* TYPE::GetTypeInfo() const \
	{ \
		return TYPE::SelfTypeTraits::GetTypeInfo(); \
	} \
	uint32_t TYPE::GetTypeId() const \
	{ \
		return TYPE::TypeNameAndId::TypeId; \
	} \
	bool TYPE::Serialize(::Introspection::ISerializer& serializer) const \
	{ \
		return TYPE::SelfTypeTraits::Serialize(serializer, *this); \
	} \
	TYPE* TYPE::NullCast( void ) \
	{ \
		return reinterpret_cast<TYPE*>(NULL); \
	} \
	void TYPE::AddMember( const ::std::string& name, const ::std::wstring& wname, uintptr_t offset, const ::Introspection::TypeInfo *typeInfo, bool isPointer, bool serializable ) \
	{ \
		return TYPE::SelfTypeTraits::AddMember( name, wname, offset, typeInfo, isPointer, serializable ); \
	} \
	void ::Introspection::TypeInfoTraits<TYPE>::RegisterMembers( void ) \
	{ \
		printf("RegisterMembers for %s.\n", #TYPE);\
		TYPE::RegisterMembers( ); \
	} \
	void TYPE::RegisterMembers( void )

#define DEFINE_TYPEINFO_BASICTYPE( TYPE ) \
	::Introspection::TypeInfoTraits<Introspection::RemoveQualifier<TYPE>::type> NAME_GENERATOR( )( nullptr, ::Introspection::TL_IndexOf<::Introspection::BasicTypeList, TYPE>::value, #TYPE, L#TYPE, sizeof( TYPE ) ); \
	void ::Introspection::TypeInfoTraits<Introspection::RemoveQualifier<TYPE>::type>::RegisterMembers( void ) \
	{ \
		printf("RegisterMembers for %s.\n", #TYPE);\
	}

#define ADD_MEMBER( MEMBER, SERIALIZABLE ) \
	ADD_MEMBER2( MEMBER, ::Introspection::RemoveQualifier<decltype( NullCast( )->MEMBER )>::type, SERIALIZABLE )

#define ADD_MEMBER2( MEMBER, MEMBERTYPE, SERIALIZABLE ) \
	AddMember( #MEMBER, L#MEMBER, (uintptr_t)(&(NullCast( )->MEMBER)), TYPEINFO_TYPE(::std::remove_pointer<MEMBERTYPE>::type), ::std::is_pointer<MEMBERTYPE>::value, SERIALIZABLE)

//
// TYPEINFO_TYPE
// Purpose: Retrieves the proper TypeInfo instance of an object by type.
//
#define TYPEINFO_TYPE( TYPE ) (::Introspection::TypeInfoTraits<std::remove_pointer<::Introspection::RemoveQualifier<TYPE>::type>::type>::GetTypeInfo( ))

//
// TYPEINFO_OBJECT
// Purpose: Retrieves the proper TypeInfo instance of an object by an object's type.
//
#define TYPEINFO_OBJECT( OBJECT ) (::Introspection::TypeInfoTraits<::std::remove_pointer<::Introspection::RemoveQualifier<decltype( OBJECT )>::type>::type>::GetTypeInfo( ))

//
// TYPEINFO_STR
// Purpose : Finds a TypeInfo instance by string name
//
#define TYPEINFO_STR( STRING ) (::Introspection::TypeInfoManager::GetTypeInfo( STRING ))

//
// TYPEOF
// Purpose : Get the type of an object
//
#define TYPEOF(OBJECT) (::Introspection::TypeInfoTraits<::Introspection::RemoveQualifier<decltype( OBJECT )>::type>::type)


