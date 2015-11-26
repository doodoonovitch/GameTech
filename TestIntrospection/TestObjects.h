#pragma once


class S : public Introspection::ObjectBase
{
public:
	S();
	~S();

	DECLARE_TYPEINFO(S, Introspection::ObjectBase::TypeNameAndId::TypeId + 2, Introspection::ObjectBase);

	void SetStr(const char* str);

	void InitTestValues();

private:
	char* data;
	size_t len;
};


class TestObject : public Introspection::ObjectBase
{
public:
	TestObject();
	virtual ~TestObject() override;

	DECLARE_TYPEINFO(TestObject, Introspection::ObjectBase::TypeNameAndId::TypeId + 1, Introspection::ObjectBase);

	void InitTestValues();

private:

	bool _boolMember;
	char _charMember;
	wchar_t _wchar_tMember;
	float _floatMember;
	double _doubleMember;

	int8_t _int8_tMember;
	uint8_t _uint8_tMember;
	int16_t _int16_tMember;
	uint16_t _uint16_tMember;
	int32_t _int32_tMember;
	uint32_t _uint32_tMember;
	int64_t _int64_tMember;
	uint64_t _uint64_tMember;

	std::string _stringMember;
	std::wstring _wstringMember;

	uint32_t _uint32_tArray5[5];
};

class TestObject2 : public TestObject
{
public:
	TestObject2();
	virtual ~TestObject2() override;

	DECLARE_TYPEINFO(TestObject2, Introspection::ObjectBase::TypeNameAndId::TypeId + 3, TestObject);

	void InitTestValues();

private:

	S* _ptr_S;

	bool* _ptr_boolMember;
	char* _ptr_charMember;
	wchar_t* _ptr_wchar_tMember;
	float* _ptr_floatMember;
	double* _ptr_doubleMember;

	int8_t* _ptr_int8_tMember;
	uint8_t* _ptr_uint8_tMember;
	int16_t* _ptr_int16_tMember;
	uint16_t* _ptr_uint16_tMember;
	int32_t* _ptr_int32_tMember;
	uint32_t* _ptr_uint32_tMember;
	int64_t* _ptr_int64_tMember;
	uint64_t* _ptr_uint64_tMember;

	std::string* _ptr_stringMember;
	std::wstring* _ptr_wstringMember;

	uint32_t* _ptr_uint32_tArray5[5];

	wchar_t* _ptr_wchar_tArray[3];

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

	const char* _c_string = "c string";
	const wchar_t* _c_wstring = L"c wstring";

	char* _nullptr_char = nullptr;
	wchar_t* _nullptr_wchar = nullptr;
	std::string * _nullptr_string = nullptr;
	std::wstring * _nullptr_wstring = nullptr;

	TestDerivedObject::A* _nullptr_A = nullptr;
	TestDerivedObject::A* _nullptr_A3[3] = { nullptr, nullptr, nullptr };

	TestDerivedObject::A* _ptr_ABC[3] = { new TestDerivedObject::A(), new TestDerivedObject::B(), new TestDerivedObject::C() };
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
