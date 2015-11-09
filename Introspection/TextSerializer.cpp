#include "stdafx.h"
#include "Introspection.h"


namespace  Introspection
{



TextSerializerHelper::TextSerializerHelper()
	: _level(0)
{

}

TextSerializerHelper::~TextSerializerHelper()
{

}

void TextSerializerHelper::Padding(std::wostream& os)
{
	for (size_t i = 0; i < _level; ++i)
	{
		os << L"\t";
	}
}


bool TextSerialization::Serialize(ObjectBase const & object)
{
	const TypeInfo* typeInfo = object.GetTypeInfo();
	assert(typeInfo != nullptr);

	TextSerializerHelper& helper = TextSerializerHelper::Get();

	_stream << L"{ ";

	bool first = true;
	for (auto mi : typeInfo->GetMembers())
	{
		if (mi->GetSerializable())
		{
			if (!first)
				_stream << L", ";
			else
				first = false;

			auto miTypeInfo = mi->GetTypeInfo();
			_stream << mi->GetWName().c_str() << L" : ";
			object.SerializeMember(*this, mi);
		}
	}

	_stream << L"} ";

	return true;
}

bool TextSerialization::Serialize(bool value)
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(char value)
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(wchar_t value)
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(float value)
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(double value)
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(int8_t value) 
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(int16_t value) 
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(int32_t value) 
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(int64_t value) 
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(uint8_t value) 
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(uint16_t value) 
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(uint32_t value) 
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(uint64_t value)
{
	return InternalSerialize(value);
}

bool TextSerialization::Serialize(std::string const & value)
{
	_stream << L"\"" << value.c_str() << L"\"";
	return true;
}

bool TextSerialization::Serialize(std::wstring const & value)
{
	_stream << L"\"" << value.c_str() << L"\"";
	return true;
}


bool TextSerialization::Serialize(bool const * const values)
{
	return InternalSerializePointer(values);
}

bool TextSerialization::Serialize(char const * const value)
{
	if (value == nullptr)
		return SerializeNull();
	else
		_stream << L"\"" << value << L"\"";
	return true;
}

bool TextSerialization::Serialize(wchar_t const * const value)
{
	if (value == nullptr)
		return SerializeNull();
	else
		_stream << L"\"" << value << L"\"";
	return true;
}

bool TextSerialization::Serialize(float const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(double const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(int8_t const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(int16_t const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(int32_t const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(int64_t const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(uint8_t const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(uint16_t const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(uint32_t const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(uint64_t const * const value)
{
	return InternalSerializePointer(value);
}

bool TextSerialization::Serialize(std::string const * const value)
{
	if (value == nullptr)
		return SerializeNull();
	else
		return Serialize(*value);
}

bool TextSerialization::Serialize(std::wstring const * const value)
{
	if (value == nullptr)
		return SerializeNull();
	else
		return Serialize(*value);
}

bool TextSerialization::Serialize(ObjectBase const * const value)
{
	if (value == nullptr)
		return SerializeNull();
	else
		return Serialize(*value);
}

bool TextSerialization::SerializeArray(bool const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(char const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(wchar_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(float const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(double const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}


bool TextSerialization::SerializeArray(int8_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(int16_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(int32_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(int64_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(uint8_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(uint16_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(uint32_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(uint64_t const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}


bool TextSerialization::SerializeArray(std::string const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(std::wstring const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}


bool TextSerialization::SerializeArray(ObjectBase const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}


bool TextSerialization::SerializeArray(char const * const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}

bool TextSerialization::SerializeArray(wchar_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializeArray(values, itemCount);
}


bool TextSerialization::SerializeArray(bool const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(float const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(double const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}


bool TextSerialization::SerializeArray(int8_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(int16_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(int32_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(int64_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(uint8_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(uint16_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(uint32_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(uint64_t const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}


bool TextSerialization::SerializeArray(std::string const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}

bool TextSerialization::SerializeArray(std::wstring const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}


bool TextSerialization::SerializeArray(ObjectBase const * const * const values, uint32_t itemCount)
{
	return InternalSerializePointerArray(values, itemCount);
}





} // namespace  Introspection
