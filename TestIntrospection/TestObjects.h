#pragma once


class TestObject
{
public:
	TestObject(int id);
	~TestObject();

	DECLARE_TYPEINFO(TestObject);

private:
	int ID;
	bool active;
	double x;

};

class S
{
public:
	S(const char *src);
	~S();

	bool IsEqual(const S& rhs) const;
	void Concat(const S& rhs);
	friend std::ostream& operator<<(std::ostream &os, S &rhs);

	DECLARE_TYPEINFO(S);

private:
	const char *data;
	unsigned len;
};