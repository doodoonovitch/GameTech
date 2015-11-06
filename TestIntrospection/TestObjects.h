#pragma once


class S : public Introspection::ObjectBase
{
public:
	S(const char* src = "");
	~S();

	DECLARE_TYPEINFO(S, Introspection::ObjectBase::TypeNameAndId::TypeId + 2, Introspection::ObjectBase);

private:
	/*const*/ char* data;
	size_t len;
};


class TestObject : public Introspection::ObjectBase
{
public:
	TestObject();
	virtual ~TestObject() override;

	DECLARE_TYPEINFO(TestObject, Introspection::ObjectBase::TypeNameAndId::TypeId + 1, Introspection::ObjectBase);

private:

	bool _boolMember = true;
	char _charMember = 'c';
	wchar_t _wchar_tMember = 'W';
	float _floatMember = 0.001f;
	double _doubleMember = 0.000000001;

	int8_t _int8_tMember = -8;
	uint8_t _uint8_tMember = 8;
	int16_t _int16_tMember = -16;
	uint16_t _uint16_tMember = 16;
	int32_t _int32_tMember = -32;
	uint32_t _uint32_tMember = 32;
	int64_t _int64_tMember = -64;
	uint64_t _uint64_tMember = 64;

	std::string _stringMember = "string";
	std::wstring _wstringMember = L"wstring";
};

class TestObject2 : public TestObject
{
public:
	TestObject2();
	virtual ~TestObject2() override;

	DECLARE_TYPEINFO(TestObject2, Introspection::ObjectBase::TypeNameAndId::TypeId + 3, TestObject);

private:

	S* _ptr_S = new S("object S as member variable (pointer)");

	bool* _ptr_boolMember = new bool(false);
	char* _ptr_charMember = _strdup("char*");
	wchar_t* _ptr_wchar_tMember = _wcsdup(L"wchar_t*");
	float* _ptr_floatMember = new float(0.001f);
	double* _ptr_doubleMember = new double(0.000000001);

	int8_t* _ptr_int8_tMember = new int8_t(-8);
	uint8_t* _ptr_uint8_tMember = new uint8_t(8);
	int16_t* _ptr_int16_tMember = new int16_t(-16);
	uint16_t* _ptr_uint16_tMember = new uint16_t(16);
	int32_t* _ptr_int32_tMember = new int32_t(-32);
	uint32_t* _ptr_uint32_tMember = new uint32_t(32);
	int64_t* _ptr_int64_tMember = new int64_t(-64);
	uint64_t* _ptr_uint64_tMember = new uint64_t(64);

	std::string* _ptr_stringMember = new std::string("string");
	std::wstring* _ptr_wstringMember = new std::wstring(L"wstring");

};

namespace TestDerivedObject
{
	class A : public::Introspection::ObjectBase
	{
	public:
		A() {};
		virtual ~A() {}

		DECLARE_TYPEINFO(TestDerivedObject::A, Introspection::ObjectBase::TypeNameAndId::TypeId + 4, Introspection::ObjectBase);

	private:
		std::wstring _object_A_member = L"Object A member";
	};

	class B : public A
	{
	public:
		B() : A() {};
		virtual ~B() {}

		DECLARE_TYPEINFO(TestDerivedObject::B, Introspection::ObjectBase::TypeNameAndId::TypeId + 5, Introspection::ObjectBase);

	private:
		std::wstring _object_B_member = L"Object B member inherits fromobject 'A'";
	};

	class C : public B
	{
	public:
		C() : B() {};
		virtual ~C() {}

		DECLARE_TYPEINFO(TestDerivedObject::C, Introspection::ObjectBase::TypeNameAndId::TypeId + 6, Introspection::ObjectBase);

	private:
		std::wstring _object_C_member = L"Object C member";
	};
}


class X : public::Introspection::ObjectBase
{
public:
	X() {};
	virtual ~X() {}

	DECLARE_TYPEINFO(X, Introspection::ObjectBase::TypeNameAndId::TypeId + 7, Introspection::ObjectBase);

private:
	std::wstring _object_X_member = L"Object X member";
};

class Y : public::Introspection::ObjectBase
{
public:

	Y() : _ptr_A(new TestDerivedObject::C()) {};
	virtual ~Y() {}

	DECLARE_TYPEINFO(Y, Introspection::ObjectBase::TypeNameAndId::TypeId + 8, Introspection::ObjectBase);

private:
	std::wstring _object_Y_member = L"Object Y contains X and A*";

	X _object_X;
	TestDerivedObject::A* _ptr_A = nullptr;
	TestDerivedObject::B _object_B;
};
