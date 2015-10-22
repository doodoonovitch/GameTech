// TestIntrospection.cpp�: d�finit le point d'entr�e pour l'application console.
//

#include "stdafx.h"

#include "TestObjects.h"

#include "DefineBasicTypeInfo.inl"


#define PRINT_MEMBERS( TYPE ) \
  PrintMembers<TYPE>( #TYPE )

template <typename TYPE>
void PrintMembers(const char *type)
{
	const Introspection::TypeInfo* ti = TYPEINFO_TYPE(TYPE);

	std::cout << std::endl << "TypeInfo for " << type << " :" << std::endl;
	std::cout << "\t       Class name : " << ti->GetName() << std::endl;
	std::cout << "\t             Size : " << ti->GetSize() << std::endl;
	std::cout << "\t\tis a basic type :  " << ti->GetIsBasicType() << std::endl;

	std::cout << "\t{" << std::endl;
	for (auto i = TYPEINFO_TYPE(TYPE)->GetMembers().begin(); i != TYPEINFO_TYPE(TYPE)->GetMembers().end(); ++i)
	{
		const Introspection::TypeInfo* pTypeInfo = (*i)->GetTypeInfo();

		std::cout << "\t\t    Member name :  " << (*i)->GetName() << std::endl;
		std::cout << "\t\t         offset :  " << (*i)->GetOffset() << std::endl;
		std::cout << "\t\t   serializable :  " << (*i)->GetSerializable() << std::endl;

		std::cout << "\t\t      type name :  " << pTypeInfo->GetName() << std::endl;
		std::cout << "\t\tis a basic type :  " << pTypeInfo->GetIsBasicType() << std::endl;
		std::cout << "\t\tis a pointer    :  " << pTypeInfo->GetIsPointer() << std::endl;
		std::cout << std::endl;
	}
	std::cout << "\t}" << std::endl;
}

template <typename T>
void PrintMembersValue(const T& obj)
{
	const Introspection::TypeInfo* typeInfo = TYPEINFO_OBJECT(obj);
	for (auto i = typeInfo->GetMembers().begin(); i != typeInfo->GetMembers().end(); ++i)
	{
		std::cout << "\t  Name :" << (*i)->GetName() << std::endl;
		//std::cout << "\t Value :" << *(*i)->GetPtr<TYPEOF(obj)>(&obj) << std::endl;
		std::cout << std::endl;
	}
}


int main()
{
	std::cout << TYPEINFO_TYPE(int)->GetName() << std::endl;
	std::cout << TYPEINFO_TYPE(int)->GetSize() << std::endl;
	std::cout << std::endl;
	std::cout << TYPEINFO_TYPE(float)->GetName() << std::endl;
	std::cout << TYPEINFO_TYPE(float)->GetSize() << std::endl;
	std::cout << std::endl;
	std::cout << TYPEINFO_TYPE(char *)->GetName() << std::endl;
	std::cout << TYPEINFO_TYPE(char *)->GetSize() << std::endl;
	std::cout << std::endl;
	std::cout << TYPEINFO_TYPE(std::string)->GetName() << std::endl;
	std::cout << TYPEINFO_TYPE(std::string)->GetSize() << std::endl;
	std::cout << std::endl;

	TestObject obj(100, true);
	PRINT_MEMBERS(TestObject);
	std::cout << "TestObject Obj(1) members value :" << std::endl;
	PrintMembersValue(obj);

	S string("String within the S wrapper!");
	std::cout << std::endl << string << std::endl;

	PRINT_MEMBERS(S);

	const Introspection::TypeInfo *typeInfo = TYPEINFO_OBJECT("asdf");
	if (typeInfo == nullptr || typeInfo != TYPEINFO_TYPE(const char *))
		std::cout << "ERROR!";

	// Ensure string literal typeInfo is supported
	if (TYPEINFO_TYPE(const char *) != TYPEINFO_OBJECT("Testing with a string literal!"))
		std::cout << "ERROR2!";

	std::wcout << "Serialization :" << std::endl;
	Introspection::TextSerialization txtSer(std::wcout);
	TYPEINFO_TYPE(TestObject)->Serialize(txtSer, &obj);


	getchar();

    return 0;
}

