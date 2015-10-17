#ifndef INTROSPECTION_MEMBERINFO_H
#define INTROSPECTION_MEMBERINFO_H

#pragma once

namespace Introspection
{

	class TypeInfo;

class MemberInfo
{
public:

	MemberInfo(const std::string& name, size_t offset, const TypeInfo* typeInfo);

	~MemberInfo();


	const std::string& GetName(void) const { return _name; }

	size_t GetOffset(void) const { return _offset; }

	const TypeInfo *GetType(void) const { return _pType; }


private:

	std::string _name;
	size_t _offset;
	const TypeInfo *_pType;

};


} // namespace  Introspection
#endif // INTROSPECTION_MEMBERINFO_H