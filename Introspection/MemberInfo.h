#ifndef INTROSPECTION_MEMBERINFO_H
#define INTROSPECTION_MEMBERINFO_H

#pragma once

namespace Introspection
{


class MemberInfo
{
public:

	MemberInfo(const std::string& name, const std::wstring& wname, uintptr_t offset, const TypeInfo* typeInfo, bool isPointer, uint32_t extent, bool serializable = false);

	~MemberInfo();


	const std::string& GetName(void) const { return _name; }
	const std::wstring& GetWName(void) const { return _wname; }

	uintptr_t GetOffset(void) const { return _offset; }

	const TypeInfo *GetTypeInfo(void) const { return _pType; }

	bool GetIsPointer(void) const { return _isPointer; }
	uint32_t GetExtent(void) const { return _extent; }

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
	uint32_t _extent;
	bool _isPointer;
	bool _serializable;

};


} // namespace  Introspection
#endif // INTROSPECTION_MEMBERINFO_H