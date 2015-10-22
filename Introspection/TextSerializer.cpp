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


bool TextSerialization::SerializeObject(const TypeInfo* typeInfo, const void* object)
{
	assert(typeInfo != nullptr);

	TextSerializerHelper& helper = TextSerializerHelper::Get();
	helper.Padding(_stream);
	_stream << L"{" << std::endl;
	helper.IncrementLevel();
	for (auto mi : typeInfo->GetMembers())
	{
		if (mi->GetSerializable())
		{
			auto miTypeInfo = mi->GetTypeInfo();
			_stream << mi->GetWName() << L" : ";
			miTypeInfo->Serialize(*this, mi->GetPtr(object));
			_stream << std::endl;
		}
	}
	helper.DecrementLevel();
	_stream << "}" << std::endl;

	return true;
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


bool TextSerialization::SerializeBasicType(const int8_t * value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const int16_t * value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const int32_t * value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const int64_t * value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const uint8_t * value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const uint16_t * value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const uint32_t * value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const uint64_t * value)
{
	return Serialize(value);
}


bool TextSerialization::SerializeBasicType(bool value) 
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


bool TextSerialization::SerializeBasicType(const bool * value)
{
	return Serialize(*value);
}

bool TextSerialization::SerializeBasicType(const float * value)
{
	return Serialize(*value);
}

bool TextSerialization::SerializeBasicType(const double * value)
{
	return Serialize(*value);
}


bool TextSerialization::SerializeBasicType(const std::string& value) 
{
	_stream << value.c_str();
	return true;
}

bool TextSerialization::SerializeBasicType(const std::wstring& value) 
{
	_stream << value.c_str();
	return true;
}


bool TextSerialization::SerializeBasicType(char value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const char* value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(wchar_t value)
{
	return Serialize(value);
}

bool TextSerialization::SerializeBasicType(const wchar_t* value)
{
	return Serialize(value);
}



} // namespace  Introspection
