#ifndef INTROSPECTION_MEMBERINFO_H
#define INTROSPECTION_MEMBERINFO_H

#pragma once

namespace Introspection
{

	class TypeInfo;

class MemberInfo
{
public:

	MemberInfo(const std::string& name, const std::wstring& wname, uintptr_t offset, const TypeInfo* typeInfo, bool isPointer, bool serializable = false);

	~MemberInfo();


	const std::string& GetName(void) const { return _name; }
	const std::wstring& GetWName(void) const { return _wname; }

	uintptr_t GetOffset(void) const { return _offset; }

	const TypeInfo *GetTypeInfo(void) const { return _pType; }

	bool GetIsPointer(void) const { return _isPointer; }

	bool GetSerializable(void) const { return _serializable; }

	uintptr_t GetPtr(const void* obj) const
	{
		return (uintptr_t)(((char *)(obj)) + (_offset));
	}

private:

	uintptr_t _offset;
	const TypeInfo *_pType;
	std::string _name;
	std::wstring _wname;
	bool _isPointer;
	bool _serializable;

};


} // namespace  Introspection
#endif // INTROSPECTION_MEMBERINFO_H