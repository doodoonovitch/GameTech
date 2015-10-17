#include "stdafx.h"
#include "TestObjects.h"


DEFINE_TYPEINFO(TestObject)
{
	ADD_MEMBER(ID);
	ADD_MEMBER(active);
	ADD_MEMBER(x);
}


TestObject::TestObject(int id)
	: ID(0)
	, active(true)
{
}


TestObject::~TestObject()
{
}


DEFINE_TYPEINFO(S)
{
	ADD_MEMBER(data);
	ADD_MEMBER(len);
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