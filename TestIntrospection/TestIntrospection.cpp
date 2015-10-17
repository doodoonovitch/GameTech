// TestIntrospection.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

#include "TestObjects.h"

#include "DefineBasicTypeInfo.inl"


#define PRINT_MEMBERS( TYPE ) \
  PrintMembers<TYPE>( #TYPE )

template <typename TYPE>
void PrintMembers(const char *type)
{
	std::cout << std::endl << "TypeInfo for " << type << " :" << std::endl;
	std::cout << "\tClass name : " << TYPEINFO_TYPE(TYPE)->GetName() << std::endl;
	std::cout << "\tSize       : " << TYPEINFO_TYPE(TYPE)->GetSize() << std::endl;

	std::cout << "\t{" << std::endl;
	for (auto i = TYPEINFO_TYPE(TYPE)->GetMembers().begin(); i != TYPEINFO_TYPE(TYPE)->GetMembers().end(); ++i)
	{
		std::cout << "\t\tMember name :  " << i->second->GetName() << std::endl;
		std::cout << "\t\t     offset :  " << i->second->GetOffset() << std::endl;
	}
	std::cout << "\t}" << std::endl;
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

	PRINT_MEMBERS(TestObject);

	S string("String within the S wrapper!");
	std::cout << std::endl << string << std::endl;

	PRINT_MEMBERS(S);

	const Introspection::TypeInfo *typeInfo = TYPEINFO_OBJECT("asdf");
	if (typeInfo == nullptr || typeInfo != TYPEINFO_TYPE(const char *))
		std::cout << "ERROR!";

	// Ensure string literal typeInfo is supported
	if (TYPEINFO_TYPE(const char *) != TYPEINFO_OBJECT("Testing with a string literal!"))
		std::cout << "ERROR2!";

	getchar();

    return 0;
}

