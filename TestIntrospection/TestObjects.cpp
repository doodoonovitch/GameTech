#include "stdafx.h"
#include "TestObjects.h"


DEFINE_TYPEINFO(S)
{
	ADD_MEMBER(data, true);
	ADD_MEMBER(len, true);
}

S::S(const char *src) : data((char*)src)
{
}

S::~S()
{
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
}


TestObject::~TestObject()
{
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
	for (uint32_t i = 0; i < 5; ++i)
	{
		_ptr_uint32_tArray5[i] = new uint32_t(10 + i);
	}
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
		delete _ptr_wchar_tArray[i];

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
