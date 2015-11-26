#ifndef INTROSPECTION_VARIANT_H
#define INTROSPECTION_VARIANT_H

#pragma once

namespace Introspection
{
	class TypeInfo;


class Variant
{
public:

	template <typename T>
	Variant(const T& value) 
		: Variant(TYPEINFO_TYPE(T), const_cast<T *>(&value))
	{
	}

	Variant(const Variant& rhs);
	Variant(const TypeInfo* typeInfo, void* data);

	~Variant();

	Variant& operator=(const Variant& rhs);

	template <typename T>
	Variant& operator=(const T& rhs)
	{
		_pTypeInfo = TYPEINFO_TYPE(T);
		_pData = const_cast<T *>(&rhs);
		return *this;
	}


	const TypeInfo *GetTypeInfo(void) const { return _pTypeInfo; }

	template <typename T>
	T& GetValue(void)
	{
		return *reinterpret_cast<T *>(data);
	}

	template <typename T>
	const T& GetValue(void) const
	{
		return *reinterpret_cast<T *>(data);
	}


	void *GetData(void) const { return _pData; }


	void Serialize(std::ostream& os) const;


protected:
	Variant();

	const TypeInfo* _pTypeInfo;
	void* _pData;
};


} // namespace  Introspection
#endif // INTROSPECTION_VARIANT_H