#include "stdafx.h"
#include "TestObjects.h"


DEFINE_TYPEINFO(S)
{
	ADD_MEMBER(data, true);
	ADD_MEMBER(len, true);
}

S::S() : data(nullptr), len(0)
{
}

S::~S()
{
	if (data != nullptr)
	{
		free(data);
	}
}

void S::SetStr(const char* str)
{
	if (data != nullptr)
	{
		free(data);
	}

	if (str != nullptr)
	{
		data = _strdup(str);
		len = std::strlen(data);

	}
	else
	{
		data = nullptr;
		len = 0;
	}

}

void S::InitTestValues()
{
	SetStr("c string (char *) - test value");
}



//




DEFINE_TYPEINFO(TestObject)
{
	ADD_MEMBER(_boolMember, true);
	ADD_MEMBER(_charMember, true);
	ADD_MEMBER(_wchar_tMember, true);
	ADD_MEMBER(_floatMember, true);
	ADD_MEMBER(_doubleMember, true);

	ADD_MEMBER(_int8_tMember, true);
	ADD_MEMBER(_uint8_tMember, true);
	ADD_MEMBER(_int16_tMember, true);
	ADD_MEMBER(_uint16_tMember, true);
	ADD_MEMBER(_int32_tMember, true);
	ADD_MEMBER(_uint32_tMember, true);
	ADD_MEMBER(_int64_tMember, true);
	ADD_MEMBER(_uint64_tMember, true);

	ADD_MEMBER(_stringMember, true);
	ADD_MEMBER(_wstringMember, true);

	ADD_MEMBER(_uint32_tArray5, true);
}


TestObject::TestObject()
{
	_boolMember = false;
	_charMember = 0;
	_wchar_tMember = 0;
	_floatMember = 0.0f;
	_doubleMember = 0.0;

	_int8_tMember = 0;
	_uint8_tMember = 0;
	_int16_tMember = 0;
	_uint16_tMember = 0;
	_int32_tMember = 0;
	_uint32_tMember = 0;
	_int64_tMember = 0;
	_uint64_tMember = 0;

	std::memset(_uint32_tArray5, 0, sizeof(_uint32_tArray5));
}


TestObject::~TestObject()
{
}

void TestObject::InitTestValues()
{
	_boolMember = true;
	_charMember = 'c';
	_wchar_tMember = 'W';
	_floatMember = 0.001f;
	_doubleMember = 0.000000001;

	_int8_tMember = -8;
	_uint8_tMember = 8;
	_int16_tMember = -16;
	_uint16_tMember = 16;
	_int32_tMember = -32;
	_uint32_tMember = 32;
	_int64_tMember = -64;
	_uint64_tMember = 64;

	_stringMember = "string";
	_wstringMember = L"wstring";

	for (int i = 0; i < 5; ++i)
		_uint32_tArray5[i] = i + 1;
}



//




DEFINE_TYPEINFO(TestObject2)
{
	ADD_MEMBER(_ptr_S, true);

	ADD_MEMBER(_ptr_boolMember, true);
	ADD_MEMBER(_ptr_charMember, true);
	ADD_MEMBER(_ptr_wchar_tMember, true);
	ADD_MEMBER(_ptr_floatMember, true);
	ADD_MEMBER(_ptr_doubleMember, true);

	ADD_MEMBER(_ptr_int8_tMember, true);
	ADD_MEMBER(_ptr_uint8_tMember, true);
	ADD_MEMBER(_ptr_int16_tMember, true);
	ADD_MEMBER(_ptr_uint16_tMember, true);
	ADD_MEMBER(_ptr_int32_tMember, true);
	ADD_MEMBER(_ptr_uint32_tMember, true);
	ADD_MEMBER(_ptr_int64_tMember, true);
	ADD_MEMBER(_ptr_uint64_tMember, true);

	ADD_MEMBER(_ptr_stringMember, true);
	ADD_MEMBER(_ptr_wstringMember, true);

	ADD_MEMBER(_ptr_uint32_tArray5, true);
	ADD_MEMBER(_ptr_wchar_tArray, true);
}




//




TestObject2::TestObject2()
{
	_ptr_S = nullptr;

	_ptr_boolMember = nullptr;
	_ptr_charMember = nullptr;
	_ptr_wchar_tMember = nullptr;
	_ptr_floatMember = nullptr;
	_ptr_doubleMember = nullptr;

	_ptr_int8_tMember = nullptr;
	_ptr_uint8_tMember = nullptr;
	_ptr_int16_tMember = nullptr;
	_ptr_uint16_tMember = nullptr;
	_ptr_int32_tMember = nullptr;
	_ptr_uint32_tMember = nullptr;
	_ptr_int64_tMember = nullptr;
	_ptr_uint64_tMember = nullptr;

	_ptr_stringMember = nullptr;
	_ptr_wstringMember = nullptr;

	std::memset(_ptr_uint32_tArray5, 0, sizeof(_ptr_uint32_tArray5));
	std::memset(_ptr_wchar_tArray, 0, sizeof(_ptr_wchar_tArray));
}


TestObject2::~TestObject2()
{
	delete _ptr_boolMember;
	delete _ptr_charMember;
	delete _ptr_wchar_tMember;
	delete _ptr_floatMember;
	delete _ptr_doubleMember;

	delete _ptr_int8_tMember;
	delete _ptr_uint8_tMember;
	delete _ptr_int16_tMember;
	delete _ptr_uint16_tMember;
	delete _ptr_int32_tMember;
	delete _ptr_uint32_tMember;
	delete _ptr_int64_tMember;
	delete _ptr_uint64_tMember;

	delete _ptr_stringMember;
	delete _ptr_wstringMember;

	for (uint32_t i = 0; i < 5; ++i)
		delete _ptr_uint32_tArray5[i];

	for (uint32_t i = 0; i < 3; ++i)
	{
		free(_ptr_wchar_tArray[i]);
	}
		

}

void TestObject2::InitTestValues()
{
	_ptr_S = new S(); _ptr_S->SetStr("object S as member variable (pointer)");

	_ptr_boolMember = new bool(false);
	_ptr_charMember = _strdup("char*");
	_ptr_wchar_tMember = _wcsdup(L"wchar_t*");
	_ptr_floatMember = new float(0.001f);
	_ptr_doubleMember = new double(0.000000001);

	_ptr_int8_tMember = new int8_t(-8);
	_ptr_uint8_tMember = new uint8_t(8);
	_ptr_int16_tMember = new int16_t(-16);
	_ptr_uint16_tMember = new uint16_t(16);
	_ptr_int32_tMember = new int32_t(-32);
	_ptr_uint32_tMember = new uint32_t(32);
	_ptr_int64_tMember = new int64_t(-64);
	_ptr_uint64_tMember = new uint64_t(64);

	_ptr_stringMember = new std::string("string");
	_ptr_wstringMember = new std::wstring(L"wstring");

	for (uint32_t i = 0; i < 5; ++i)
	{
		_ptr_uint32_tArray5[i] = new uint32_t(10 + i);
	}

	_ptr_wchar_tArray[0] = _wcsdup(L"wchar_t string 1");
	_ptr_wchar_tArray[1] = _wcsdup(L"wchar_t string 2");
	_ptr_wchar_tArray[2] = _wcsdup(L"wchar_t string 3");
}



DEFINE_TYPEINFO(TestDerivedObject::A)
{
	ADD_MEMBER(_object_A_member, true);
}

DEFINE_TYPEINFO(TestDerivedObject::B)
{
	ADD_MEMBER(_object_B_member, true);
}

DEFINE_TYPEINFO(TestDerivedObject::C)
{
	ADD_MEMBER(_object_C_member, true);
}


DEFINE_TYPEINFO(X)
{
	ADD_MEMBER(_object_X_member, true);
	ADD_MEMBER(_c_string, true);
	ADD_MEMBER(_c_wstring, true);
	ADD_MEMBER(_nullptr_char, true);
	ADD_MEMBER(_nullptr_wchar, true);
	ADD_MEMBER(_nullptr_string, true);
	ADD_MEMBER(_nullptr_wstring, true);
	ADD_MEMBER(_nullptr_A, true);
	ADD_MEMBER(_nullptr_A3, true);
	ADD_MEMBER(_ptr_ABC, true);
}

DEFINE_TYPEINFO(Y)
{
	ADD_MEMBER(_object_X, true);
	ADD_MEMBER(_ptr_A, true);
	ADD_MEMBER(_object_B, true);
}
