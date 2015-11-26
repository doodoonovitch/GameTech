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


void TextSerialization::Serialize(ObjectBase const & object)
{
	const TypeInfo* typeInfo = object.GetTypeInfo();
	assert(typeInfo != nullptr);

	_stream << std::endl;
	_helper.Padding(_stream);
	_stream << L"{ " << std::endl;

	_helper.IncrementLevel();

	size_t memberCount = 0;
	SerializeObjectMembers(object, typeInfo, memberCount);
	if (memberCount > 0)
		_stream << std::endl;

	_helper.DecrementLevel();

	_helper.Padding(_stream);
	_stream << L"} " << std::endl;
}

void TextSerialization::SerializeObjectMembers(ObjectBase const & object, const TypeInfo* typeInfo, size_t& memberCount) 
{
	const TypeInfo* parent = typeInfo->GetParent();
	if (parent != nullptr)
		SerializeObjectMembers(object, parent, memberCount);

	for (auto mi : typeInfo->GetMembers())
	{
		if (mi->GetSerializable())
		{
			auto miTypeInfo = mi->GetTypeInfo();

			if (memberCount != 0)
				_stream << L", " << std::endl;
			else
				_stream << std::endl;

			_helper.Padding(_stream);
			_stream << L"\"" << mi->GetWName().c_str() << L"\" : ";
			object.SerializeMember(*this, mi);

			++memberCount;
		}
	}
}

void TextSerialization::Serialize(bool value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(char value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(wchar_t value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(float value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(double value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(int8_t value) 
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(int16_t value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(int32_t value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(int64_t value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(uint8_t value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(uint16_t value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(uint32_t value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(uint64_t value)
{
	InternalSerialize(value);
}

void TextSerialization::Serialize(std::string const & value)
{
	_stream << L"\"" << value.c_str() << L"\"";
}

void TextSerialization::Serialize(std::wstring const & value)
{
	_stream << L"\"" << value.c_str() << L"\"";
}


void TextSerialization::Serialize(bool const * const values)
{
	InternalSerializePointer(values);
}

void TextSerialization::Serialize(char const * const value)
{
	if (value == nullptr)
		SerializeNull();
	else
		_stream << L"\"" << value << L"\"";
}

void TextSerialization::Serialize(wchar_t const * const value)
{
	if (value == nullptr)
		SerializeNull();
	else
		_stream << L"\"" << value << L"\"";
}

void TextSerialization::Serialize(float const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(double const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(int8_t const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(int16_t const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(int32_t const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(int64_t const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(uint8_t const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(uint16_t const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(uint32_t const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(uint64_t const * const value)
{
	InternalSerializePointer(value);
}

void TextSerialization::Serialize(std::string const * const value)
{
	if (value == nullptr)
		return SerializeNull();
	else
		return Serialize(*value);
}

void TextSerialization::Serialize(std::wstring const * const value)
{
	if (value == nullptr)
		SerializeNull();
	else
		Serialize(*value);
}

void TextSerialization::Serialize(ObjectBase const * const value)
{
	if (value == nullptr)
		SerializeNull();
	else
		Serialize(*value);
}

void TextSerialization::SerializeArray(bool const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(char const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(wchar_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(float const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(double const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void TextSerialization::SerializeArray(int8_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(int16_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(int32_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(int64_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(uint8_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(uint16_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(uint32_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(uint64_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void TextSerialization::SerializeArray(std::string const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(std::wstring const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void TextSerialization::SerializeArray(ObjectBase const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void TextSerialization::SerializeArray(char const * const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void TextSerialization::SerializeArray(wchar_t const * const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void TextSerialization::SerializeArray(bool const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(float const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(double const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}


void TextSerialization::SerializeArray(int8_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(int16_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(int32_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(int64_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(uint8_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(uint16_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(uint32_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(uint64_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}


void TextSerialization::SerializeArray(std::string const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void TextSerialization::SerializeArray(std::wstring const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}


void TextSerialization::SerializeArray(ObjectBase const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}





} // namespace  Introspection
