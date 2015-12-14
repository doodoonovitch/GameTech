// TestIntrospection.cpp�: d�finit le point d'entr�e pour l'application console.
//

#include "stdafx.h"

#include "TestObjects.h"


//#define PRINT_MEMBERS( TYPE ) \
//  PrintMembers<TYPE>( #TYPE )

void PrintMembers(const Introspection::TypeInfo* ti)
{
	std::wcout << std::boolalpha << std::endl ;
	std::wcout << L"Type : '" << ti->GetTypeWName() << L"' ..." << std::endl;
	std::wcout << L"\tType Id         : " << ti->GetTypeId() << std::endl;
	std::wcout << L"\tSize            : " << ti->GetTypeSize() << std::endl;
	std::wcout << L"\tis a basic type : " << ti->GetIsBasicType() << std::endl;

	std::wcout << L"\tMembers ..." << std::endl;
	for (auto mi = ti->GetMembers().begin(); mi != ti->GetMembers().end(); ++mi)
	{
		const Introspection::TypeInfo* pTypeInfo = (*mi)->GetTypeInfo();

		std::wcout << L"\t\t member name     : '" << (*mi)->GetWName() << L"' ..." << std::endl;
		std::wcout << L"\t\t offset          : " << (*mi)->GetOffset() << std::endl;
		std::wcout << L"\t\t serializable    : " << (*mi)->GetSerializable() << std::endl;
		std::wcout << L"\t\t is a pointer    : " << (*mi)->GetIsPointer() << std::endl;

		std::wcout << L"\t\t type name       : " << pTypeInfo->GetTypeWName() << std::endl;
		std::wcout << L"\t\t type Id         : " << pTypeInfo->GetTypeId() << std::endl;
		std::wcout << L"\t\t is a basic type : " << pTypeInfo->GetIsBasicType() << std::endl;
		std::wcout << std::endl;
	}
}

template <typename TYPE>
void PrintMembers()
{
	const Introspection::TypeInfo* ti = TYPEINFO_TYPE(TYPE);
	PrintMembers(ti);
}

void PrintMembers(const Introspection::ObjectBase* obj)
{
	const Introspection::TypeInfo* ti = obj->GetTypeInfo(); //TYPEINFO_TYPE(TYPE);
	PrintMembers(ti);
}

template <typename T>
void PrintTypeInfo()
{
	std::wcout << "Print type info of : '"<< typeid(T).name() << "'" << std::endl;
	const Introspection::TypeInfo* typeInfo = TYPEINFO_TYPE(T);
	std::wcout << L"Name: '" << typeInfo->GetTypeWName() << L"'" << std::endl;
	std::wcout << L"\tId: " << typeInfo->GetTypeId() << std::endl;
	std::wcout << L"\tSize: " << typeInfo->GetTypeSize() << std::endl;
	std::wcout << L"\tBasic type: " << typeInfo->GetIsBasicType() << std::endl;
	std::wcout << std::endl;
}

int main()
{

	Introspection::InitializeBasicTypes();

	std::wcout << std::boolalpha;
	std::wcout << L"Basic types... :" << std::endl;
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
	std::wcout << std::endl;
	PrintTypeInfo<int>();
	PrintTypeInfo<unsigned char>();
	PrintTypeInfo<size_t>();
	PrintTypeInfo<intmax_t>();
	std::wcout << std::endl;

	std::wcout << L"Basic type pointers :" << std::endl;
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
	std::wcout << std::endl;

	std::wcout << L"Others types :" << std::endl;
	PrintTypeInfo<TestDerivedObject::A>();
	PrintTypeInfo<TestDerivedObject::B>();
	PrintTypeInfo<TestDerivedObject::C>();
	PrintTypeInfo<X>();
	PrintTypeInfo<Y>();
	std::wcout << std::endl;

	TestObject obj1;
	obj1.InitTestValues();
	PrintMembers(&obj1);

	TestObject2 obj2;
	obj2.InitTestValues();
	PrintMembers(&obj2);

	S sObject;
	sObject.InitTestValues();
	PrintMembers(&sObject);

	PrintMembers<TestDerivedObject::A>();
	PrintMembers<TestDerivedObject::B>();
	PrintMembers<TestDerivedObject::C>();

	PrintMembers<X>();
	PrintMembers<Y>();


	//const Introspection::TypeInfo *typeInfo = TYPEINFO_OBJECT("asdf");
	//if (typeInfo == nullptr || typeInfo != TYPEINFO_TYPE(const char *))
	//	std::cout << "ERROR!";

	//// Ensure string literal typeInfo is supported
	//if (TYPEINFO_TYPE(const char *) != TYPEINFO_OBJECT("Testing with a string literal!"))
	//	std::cout << "ERROR2!";

	{
		std::wostringstream strstream;
		Introspection::TextSerialization txtSer(strstream);

		strstream << L"Serialization obj1 (TestObject):" << std::endl;
		obj1.Serialize(txtSer);

		strstream << std::endl;

		strstream << L"Serialization obj2 (TestObject2):" << std::endl;
		obj2.Serialize(txtSer);

		std::wcout << std::endl << strstream.str() << std::endl;
	}

	{
		std::wcout << std::endl;
		Introspection::TextSerialization coutSer(std::wcout);
		Y objY;
		std::wcout << L"Serialization objY:" << std::endl;
		objY.Serialize(coutSer);
	}

	{
		{
			tinyxml2::XMLDocument doc;
			Introspection::XMLSerialization ser(doc);

			obj1.Serialize(ser);

			std::wcout << L"XML Serialization obj1 (TestObject):" << std::endl;

			tinyxml2::XMLPrinter printer;
			doc.Print(&printer);

			std::wcout << printer.CStr() << std::endl << std::endl;
		}

		{
			tinyxml2::XMLDocument doc;
			Introspection::XMLSerialization ser(doc);
			obj2.Serialize(ser);

			std::wcout << L"XML Serialization obj2 (TestObject2):" << std::endl;

			tinyxml2::XMLPrinter printer;
			doc.Print(&printer);
			std::wcout << printer.CStr() << std::endl << std::endl;
		}
	}


	std::wcout << std::endl << std::endl << std::endl;
	std::wcout << std::endl << L"obj2 IsKindOf(obj1) : " << obj2.IsKindOf(&obj1);
	std::wcout << std::endl << L"obj1 IsKindOf(obj2) : " << obj1.IsKindOf(&obj2);
	std::wcout << std::endl << L"obj2 IsKindOf(S) : " << obj2.IsKindOf(&sObject);

	std::wcout << "std::remove_cv(const char*) : " << typeid(std::remove_cv<const char*>::type).name() << std::endl;
	std::wcout << "std::remove_cv(const wchar_t*) : " << typeid(std::remove_cv<const wchar_t*>::type).name() << std::endl;
	std::wcout << "std::remove_cv(const char) : " << typeid(std::remove_cv<const char>::type).name() << std::endl;
	std::wcout << "std::remove_cv(const wchar_t) : " << typeid(std::remove_cv<const wchar_t>::type).name() << std::endl;
	std::wcout << std::endl;
	std::wcout << "std::remove_cv(volatile const char*) : " << typeid(std::remove_cv<volatile const char*>::type).name() << std::endl;
	std::wcout << "std::remove_cv(volatile const wchar_t*) : " << typeid(std::remove_cv<volatile const wchar_t*>::type).name() << std::endl;
	std::wcout << "std::remove_cv(volatile const char) : " << typeid(std::remove_cv<volatile const char>::type).name() << std::endl;
	std::wcout << "std::remove_cv(volatile const wchar_t) : " << typeid(std::remove_cv<volatile const wchar_t>::type).name() << std::endl;
	std::wcout << "std::remove_cv(volatile const int8_t*) : " << typeid(std::remove_cv<volatile const int8_t*>::type).name() << std::endl;
	std::wcout << "std::remove_cv(volatile const int32_t*) : " << typeid(std::remove_cv<volatile const int32_t*>::type).name() << std::endl;
	std::wcout << std::endl;
	std::wcout << "std::remove_cv(const int8_t*) : " << typeid(std::remove_cv<const int8_t*>::type).name() << std::endl;
	std::wcout << "std::remove_cv(const int32_t*) : " << typeid(std::remove_cv<const int32_t*>::type).name() << std::endl;
	std::wcout << "std::is_const(const char*) : " << std::is_const<const char*>::value << std::endl;
	std::wcout << "std::is_const(const int8_t*) : " << std::is_const<const int8_t*>::value << std::endl;

	std::wcout << std::endl;
	std::wcout << "std::remove_pointer(const char*) : " << typeid(std::remove_pointer<const char*>::type).name() << std::endl;
	std::wcout << "std::add_pointer(std::remove_cv(std::remove_pointer(const char*))) : " << typeid(std::add_pointer<std::remove_cv<std::remove_pointer<const char*>::type>::type>::type).name() << std::endl;
	std::wcout << std::endl;

	std::wcout << "std::extent(int8_t[10]) : " << std::extent<int8_t[10]>::value << std::endl;
	std::wcout << "std::extent(int8_t) : " << std::extent<int8_t>::value << std::endl;
	std::wcout << "std::extent(int8_t*[10]) : " << std::extent<int8_t*[10]>::value << std::endl;
	std::wcout << "std::extent(int8_t*) : " << std::extent<int8_t*>::value << std::endl;
	std::wcout << "std::is_pointer(int8_t*[10]) : " << std::is_pointer<int8_t*[10]>::value << std::endl;
	std::wcout << "std::remove_pointer(int8_t*[10]) : " << typeid(std::remove_pointer<int8_t*[10]>::type).name() << std::endl;
	std::wcout << "std::remove_extent(int8_t*[10]) : " << typeid(std::remove_extent<int8_t*[10]>::type).name() << std::endl;
	std::wcout << std::endl;

	std::wcout << "std::is_same(int32_t, int) : " << std::is_same<int32_t, int>::value << std::endl;
	std::wcout << "std::is_same(Introspection::TypeInfoTraits<int32_t>, Introspection::TypeInfoTraits<int>) : " << std::is_same<Introspection::TypeInfoTraits<int32_t>, Introspection::TypeInfoTraits<int>>::value << std::endl;
	std::wcout << std::endl;

	std::wcout << "Introspection::Is_Basic_Type(int) : " << Introspection::Is_Basic_Type<int>::value << std::endl;

	std::wcout << std::endl;
	uint32_t* _ptr_uint32_tArray5[5]; _ptr_uint32_tArray5[0] = nullptr;
	std::wcout << "Introspection::GetVarType(_ptr_uint32_tArray5) : " << typeid(Introspection::GetVarType<decltype(_ptr_uint32_tArray5)>::type).name() << std::endl;
	std::wcout << "Introspection::Is_Var_Pointer(_ptr_uint32_tArray5) : " << Introspection::Is_Var_Pointer<decltype(_ptr_uint32_tArray5)>::value << std::endl;
	
	std::wistringstream wstrstream(L"str1,\t\n str2");
	wchar_t str[200];
	wstrstream >> str;
	std::wcout << L"1-str: " << str << std::endl;
	wstrstream >> str;
	std::wcout << L"2-str: " << str << std::endl;
	getchar();

    return 0;
}
