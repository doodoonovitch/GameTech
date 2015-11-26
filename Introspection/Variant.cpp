#include "stdafx.h"
#include "Introspection.h"

namespace  Introspection
{



Variant::Variant()
	: _pTypeInfo(nullptr)
	, _pData(nullptr)
{
}

Variant::Variant(const TypeInfo* typeInfo, void* data)
	: _pTypeInfo(typeInfo)
	, _pData(data)
{
}

Variant::Variant(const Variant& rhs) 
	: Variant(rhs._pTypeInfo, rhs._pData)
{
}

Variant& Variant::operator=(const Variant& rhs)
{
	if (&rhs != this)
	{
		_pTypeInfo = rhs._pTypeInfo;
		_pData = rhs._pData;
	}
	return *this;
}



Variant::~Variant()
{
}

void Variant::Serialize(std::ostream& os) const
{
	_pTypeInfo->Serialize(os, Variant(_pTypeInfo, _pData));
}



} // namespace  Introspection