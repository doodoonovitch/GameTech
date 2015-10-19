#include "stdafx.h"
#include "TestObjects.h"


DEFINE_TYPEINFO(TestObject)
{
	SET_SERIALIZATION(TestObject, Introspection::TextSerialization<TestObject>::GetInstance());
	ADD_MEMBER(ID, true);
	ADD_MEMBER(active, true);
	ADD_MEMBER(x, false);
}


TestObject::TestObject(int id)
	: ID(0)
	, active(true)
	, x(0.0)
{
}


TestObject::~TestObject()
{
}


DEFINE_TYPEINFO(S)
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