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
	const Introspection::TypeInfo* ti = TYPEINFO_TYPE(TYPE);

	std::cout << std::endl << "TypeInfo for " << type << " :" << std::endl;
	std::cout << "\t       Class name : " << ti->GetName() << std::endl;
	std::cout << "\t          Type Id : " << ti->GetTypeId() << std::endl;
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
		std::cout << "\t\t        type Id :  " << pTypeInfo->GetTypeId() << std::endl;
		std::cout << "\t\tis a basic type :  " << pTypeInfo->GetIsBasicType() << std::endl;
		std::cout << "\t\tis a pointer    :  " << pTypeInfo->GetIsPointer() << std::endl;
		std::cout << std::endl;
	}
	std::cout << "\t}" << std::endl;
}

template <typename T>
void PrintMembersValue(const T& obj)
{
	const Introspection::TypeInfo* typeInfo = obj.GetTypeInfo();
	for (auto i = typeInfo->GetMembers().begin(); i != typeInfo->GetMembers().end(); ++i)
	{
		std::cout << "\t  Name :" << (*i)->GetName() << std::endl;
		//std::cout << "\t Value :" << *(*i)->GetPtr<TYPEOF(obj)>(&obj) << std::endl;
		std::cout << std::endl;
	}
}

template <typename T>
void PrintTypeInfo()
{
	const Introspection::TypeInfo* typeInfo = TYPEINFO_TYPE(T);
	std::cout << "\tId: " << typeInfo->GetTypeId() << std::endl;
	std::cout << "\tName: " << typeInfo->GetName() << std::endl;
	std::cout << "\tSize: " << typeInfo->GetSize() << std::endl;
	std::cout << std::endl;
}

int main()
{
	std::cout << std::endl;
	std::cout << "Basic types... :" << std::endl;
	PrintTypeInfo<bool>();
	PrintTypeInfo<char>();
	PrintTypeInfo<wchar_t>();
	PrintTypeInfo<float>();
	PrintTypeInfo<double>();
	PrintTypeInfo<int8_t>();
	PrintTypeInfo<uint8_t>();
	PrintTypeInfo<int16_t>();
	PrintTypeInfo<uint16_t>();
	PrintTypeInfo<int32_t>();
	PrintTypeInfo<uint32_t>();
	PrintTypeInfo<int64_t>();
	PrintTypeInfo<uint64_t>();
	PrintTypeInfo<std::string>();
	PrintTypeInfo<std::wstring>();
	std::cout << std::endl;

	std::cout << "Basic type pointers :" << std::endl;
	PrintTypeInfo<bool*>();
	PrintTypeInfo<char*>();
	PrintTypeInfo<wchar_t*>();
	PrintTypeInfo<float*>();
	PrintTypeInfo<double*>();
	PrintTypeInfo<int8_t*>();
	PrintTypeInfo<uint8_t*>();
	PrintTypeInfo<int16_t*>();
	PrintTypeInfo<uint16_t*>();
	PrintTypeInfo<int32_t*>();
	PrintTypeInfo<uint32_t*>();
	PrintTypeInfo<int64_t*>();
	PrintTypeInfo<uint64_t*>();
	PrintTypeInfo<std::string*>();
	PrintTypeInfo<std::wstring*>();
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
	obj.Serialize(txtSer);


	getchar();

    return 0;
}

