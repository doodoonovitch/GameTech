#pragma once


class TestObject : public Introspection::ObjectBase
{
public:
	TestObject(int id, bool a);
	~TestObject();

	DECLARE_TYPEINFO(TestObject, Introspection::ObjectBase);

private:
	int32_t _Id;
	bool _active;
	double _double;
	wchar_t *_wstr;

};

class S : public Introspection::ObjectBase
{
public:
	S(const char *src);
	~S();

	bool IsEqual(const S& rhs) const;
	void Concat(const S& rhs);
	friend std::ostream& operator<<(std::ostream &os, S &rhs);

	DECLARE_TYPEINFO(S, Introspection::ObjectBase);

private:
	const char *data;
	size_t len;
};