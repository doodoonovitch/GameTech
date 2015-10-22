#pragma once


class TestObject
{
public:
	TestObject(int id, bool a);
	~TestObject();

	DECLARE_TYPEINFO(TestObject);

private:
	int32_t _Id;
	bool _active;
	double _double;
	wchar_t *_wstr;

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
	size_t len;
};