#include "stdafx.h"
#include "TestObjects.h"


DEFINE_TYPEINFO(TestObject, Introspection::ObjectBase::_typeid + 1)
{
	ADD_MEMBER(_Id, true);
	ADD_MEMBER(_active, true);
	ADD_MEMBER(_double, true);
	ADD_MEMBER(_wstr, true);
}


TestObject::TestObject(int id, bool a)
	: _Id(id)
	, _active(a)
	, _double(0.0)
	, _wstr(_wcsdup(L"Test wchar_t string"))
{
}


TestObject::~TestObject()
{
	delete [] _wstr;
}


DEFINE_TYPEINFO(S, Introspection::ObjectBase::_typeid + 2)
{
	ADD_MEMBER(data, true);
	ADD_MEMBER(len, true);
}

S::S(const char *src) : data(src)
{
}

S::~S()
{
}

bool S::IsEqual(const S& rhs) const
{
	if (strcmp(data, rhs.data) == 0)
		return true;
	return false;
}

// Untested!
void S::Concat(const S& rhs)
{
	size_t bufferSize = rhs.len + len;
	char *temp = new char[bufferSize];
	strcpy_s(temp, bufferSize, data);
	strcpy_s(temp + len, bufferSize, rhs.data);
}

std::ostream& operator<<(std::ostream &os, S &rhs)
{
	os << rhs.data;
	return os;
}