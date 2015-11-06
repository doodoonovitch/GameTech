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


bool TextSerialization::SerializeObject(const ObjectBase& object)
{
	const TypeInfo* typeInfo = object.GetTypeInfo();
	assert(typeInfo != nullptr);

	TextSerializerHelper& helper = TextSerializerHelper::Get();
	helper.Padding(_stream);
	_stream << L"{" << std::endl;
	helper.IncrementLevel();
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
	helper.DecrementLevel();
	_stream << std::endl << L"}" << std::endl;

	return true;
}

bool TextSerialization::SerializeBasicType(bool value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(char value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(wchar_t value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(float value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(double value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(int8_t value) 
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(int16_t value) 
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(int32_t value) 
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(int64_t value) 
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(uint8_t value) 
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(uint16_t value) 
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(uint32_t value) 
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(uint64_t value) 
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const std::string& value) 
{
	_stream << L"\"" << value.c_str() << L"\"";
	return true;
}

bool TextSerialization::SerializeBasicType(const std::wstring& value) 
{
	_stream << L"\"" << value.c_str() << L"\"";
	return true;
}

bool TextSerialization::SerializeBasicType(const char* value)
{
	_stream << L"\"" << value << L"\"";
	return true;
}

bool TextSerialization::SerializeBasicType(const wchar_t* value)
{
	_stream << L"\"" << value << L"\"";
	return true;
}


/*

bool TextSerialization::SerializeBasicType(const bool * value)
{
	return SerializeBasicType(*value);
}


bool TextSerialization::SerializeBasicType(const float * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const double * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const int8_t * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const int16_t * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const int32_t * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const int64_t * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const uint8_t * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const uint16_t * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const uint32_t * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const uint64_t * value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const std::string* value)
{
	return SerializeBasicType(*value);
}

bool TextSerialization::SerializeBasicType(const std::wstring* value)
{
	return SerializeBasicType(*value);
}

*/

} // namespace  Introspection
