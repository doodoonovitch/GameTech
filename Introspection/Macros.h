#pragma once

#define PASTE( _, __ )  _##__
#define GENERATE_LINE( _ ) PASTE( GENERATED_TOKEN_, _ )
#define GENERATE_FILE( _ ) PASTE( __FILE__, _ )
#define NAME_GENERATOR( ) GENERATE_FILE( __LINE__ )

//virtual bool IsKindOf(const ObjectBase* parent) const override { return TYPEINFO_TYPE(TYPE)->IsKindOf(parent->GetTypeId()); } 
#define DECLARE_TYPEINFO( TYPE, TYPEID, PARENT_TYPE ) \
	template<typename T> friend class TypeInfoTraits; \
	typedef ::Introspection::TypeInfoTraits<TYPE> SelfTypeInfoTraits; \
	typedef ::Introspection::TypeAndId<TYPE, TYPEID> TypeNameAndId; \
	typedef ::Introspection::Typelist<TYPE, PARENT_TYPE::ClassHierarchy> ClassHierarchy; \
	typedef PARENT_TYPE ParentClass; \
	typedef ParentClass::SelfTypeInfoTraits ParentTypeTraits; \
	static SelfTypeInfoTraits _typeInfoTraits; \
	virtual const ::Introspection::TypeInfo* GetTypeInfo() const override; \
	virtual uint32_t GetTypeId() const override; \
	virtual void Serialize(::Introspection::ISerializer& serializer) const override; \
	static TYPE* NullCast( void ); \
	static void RegisterMembers( void )

//template<typename TMemberType> \
//static void AddMember(const ::std::string& memberName, const ::std::wstring& wMemberName, uintptr_t memberOffset, bool serializable) \
//{ \
//SelfTypeInfoTraits::GetTypeInfo()->AddMember<TMemberType>(memberName, wMemberName, memberOffset, serializable); \
//} \

#define DEFINE_TYPEINFO( TYPE ) \
	DEFINE_TYPEINFO_2( TYPE, ParentClass::SelfTypeInfoTraits::GetTypeInfo() )

#define DEFINE_TYPEINFO_OBJECTBASE( TYPE ) \
	DEFINE_TYPEINFO_2( TYPE, nullptr )

#define DEFINE_TYPEINFO_2( TYPE, PARENT_TYPE_INFO ) \
	TYPE::SelfTypeInfoTraits TYPE::_typeInfoTraits = TYPE::SelfTypeInfoTraits(PARENT_TYPE_INFO, TYPE::TypeNameAndId::TypeId, #TYPE, L#TYPE, sizeof(TYPE)); \
	const ::Introspection::TypeInfo* TYPE::GetTypeInfo() const \
	{ \
		return TYPE::SelfTypeInfoTraits::GetTypeInfo(); \
	} \
	uint32_t TYPE::GetTypeId() const \
	{ \
		return TYPE::TypeNameAndId::TypeId; \
	} \
	void TYPE::Serialize(::Introspection::ISerializer& serializer) const \
	{ \
		TYPE::SelfTypeInfoTraits::Serialize(serializer, *this); \
	} \
	TYPE* TYPE::NullCast( void ) \
	{ \
		return reinterpret_cast<TYPE*>(NULL); \
	} \
	void TYPE::RegisterMembers( void )


#define DEFINE_TYPEINFO_BASICTYPE( TYPE ) \
	static ::Introspection::TypeInfoTraits<TYPE> NAME_GENERATOR( )( nullptr, ::Introspection::TL_IndexOf<::Introspection::BasicTypeList, TYPE>::value, #TYPE, L#TYPE, sizeof( TYPE ) );

#define ADD_MEMBER( MEMBER, SERIALIZABLE ) \
	ADD_MEMBER2( MEMBER, decltype( NullCast( )->MEMBER ), SERIALIZABLE )

#define ADD_MEMBER2( MEMBER, MEMBERTYPE, SERIALIZABLE ) \
	SelfTypeInfoTraits::GetTypeInfo()->AddMember<MEMBERTYPE>( #MEMBER, L#MEMBER, (uintptr_t)(&(NullCast( )->MEMBER)), SERIALIZABLE)

//
// TYPEINFO_TYPE
// Purpose: Retrieves the proper TypeInfo instance of an object by type.
//
#define TYPEINFO_TYPE( TYPE ) (::Introspection::TypeInfoTraits<::Introspection::RemoveQualifier<std::remove_pointer<::Introspection::RemoveQualifier<TYPE>::type>::type>::type>::GetTypeInfo( ))

//
// TYPEINFO_OBJECT
// Purpose: Retrieves the proper TypeInfo instance of an object by an object's type.
//
#define TYPEINFO_OBJECT( OBJECT ) (::Introspection::TypeInfoTraits<::Introspection::GetVarType<decltype( OBJECT )>::type>::GetTypeInfo( ))

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


