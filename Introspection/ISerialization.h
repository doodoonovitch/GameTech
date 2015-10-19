#ifndef INTROSPECTION_ISERIALIZER_H
#define INTROSPECTION_ISERIALIZER_H

#pragma once

namespace  Introspection
{


class ISerialization
{
public:

	virtual bool Serialize(std::ostream& os, const void* obj) const = 0;
	virtual bool Deserialize(std::istream& is, void* obj) const = 0;

	virtual ~ISerialization() {}

protected:
	ISerialization() {}
};


} // namespace  Introspection
#endif // INTROSPECTION_ISERIALIZER_H