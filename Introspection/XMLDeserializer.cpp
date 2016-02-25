#include "stdafx.h"
#include "Introspection.h"

#pragma warning(push)
#pragma warning( disable : 4100)

namespace  Introspection
{

	using namespace tinyxml2;


XMLDeserializer::XMLDeserializer(tinyxml2::XMLNode* rootNode)
{
	_stack.push(rootNode->ToElement());
}


XMLDeserializer::~XMLDeserializer()
{
}


void XMLDeserializer::Deserialize(ObjectBase & object)
{
	if (_stack.empty())
		return;

	auto ti = object.GetTypeInfo();

	XMLElement* elem = _stack.top();
	XMLElement * listElement = elem->FirstChildElement("Member");
	while (listElement != nullptr)
	{
		const char* memberName = listElement->Attribute("Name");
		if (memberName == nullptr)
		{
			LogError();
		}
		else
		{
			auto mi = ti->GetMember(memberName, true);
			if (mi == nullptr)
			{
				LogError();
			}
			else
			{
				_stack.push(listElement);

				object.DeserializeMember(*this, mi);

				_stack.pop();

				listElement = listElement->NextSiblingElement("Member");
			}
		}
	}
}

void XMLDeserializer::Deserialize(ObjectBase * & object)
{
	if (object == nullptr)
	{
		LogError();
		return;
	}

	Deserialize(*object);
}


void XMLDeserializer::Deserialize(bool & value)
{
	BasicTypeDeserialize(value);
}

void XMLDeserializer::Deserialize(char & value)
{
	BasicTypeDeserialize(value);
}

void XMLDeserializer::Deserialize(wchar_t & value)
{
	BasicTypeDeserialize(value);
}

void XMLDeserializer::Deserialize(float & value)
{
	if (_stack.empty())
		return;

	_stack.top()->QueryFloatText(&value);
}
void XMLDeserializer::Deserialize(double & value)
{
	if (_stack.empty())
		return;

	_stack.top()->QueryDoubleText(&value);
}

void XMLDeserializer::Deserialize(int8_t & value)
{
	BasicTypeDeserialize(value);
}
void XMLDeserializer::Deserialize(int16_t & value)
{
	BasicTypeDeserialize(value);
}
void XMLDeserializer::Deserialize(int32_t & value)
{
	BasicTypeDeserialize(value);
}
void XMLDeserializer::Deserialize(int64_t & value)
{
	BasicTypeDeserialize(value);
}
void XMLDeserializer::Deserialize(uint8_t & value)
{
	BasicTypeDeserialize(value);
}
void XMLDeserializer::Deserialize(uint16_t & value)
{
	BasicTypeDeserialize(value);
}
void XMLDeserializer::Deserialize(uint32_t & value)
{
	BasicTypeDeserialize(value);
}
void XMLDeserializer::Deserialize(uint64_t & value)
{
	BasicTypeDeserialize(value);
}

void XMLDeserializer::Deserialize(std::string & value)
{
	if (_stack.empty())
		return;

	const char* str = _stack.top()->GetText();
	value = str;
}
void XMLDeserializer::Deserialize(std::wstring & value)
{
	BasicTypeDeserialize(value);
}

void XMLDeserializer::Deserialize(char * & value)
{
	if (_stack.empty())
		return;

	const char* str = _stack.top()->GetText();
	value = _strdup(str);
}

void XMLDeserializer::Deserialize(wchar_t * & value)
{
	BasicTypeDeserialize(value);
}


void XMLDeserializer::Deserialize(bool * & value)
{
	BasicTypeDeserialize(*value);
}

void XMLDeserializer::Deserialize(float * & value)
{
	BasicTypeDeserialize(*value);
}

void XMLDeserializer::Deserialize(double * & value)
{
	BasicTypeDeserialize(*value);
}


void XMLDeserializer::Deserialize(int8_t * & value)
{
	BasicTypeDeserialize(*value);
}
void XMLDeserializer::Deserialize(int16_t * & value)
{
	BasicTypeDeserialize(*value);
}
void XMLDeserializer::Deserialize(int32_t * & value)
{
	BasicTypeDeserialize(*value);
}
void XMLDeserializer::Deserialize(int64_t * & value)
{
	BasicTypeDeserialize(*value);
}
void XMLDeserializer::Deserialize(uint8_t * & value)
{
	BasicTypeDeserialize(*value);
}
void XMLDeserializer::Deserialize(uint16_t * & value)
{
	BasicTypeDeserialize(*value);
}
void XMLDeserializer::Deserialize(uint32_t * & value)
{
	BasicTypeDeserialize(*value);
}
void XMLDeserializer::Deserialize(uint64_t * & value)
{
	BasicTypeDeserialize(*value);
}


void XMLDeserializer::Deserialize(std::string * & value) 
{
	Deserialize(*value);
}

void XMLDeserializer::Deserialize(std::wstring * & value)
{
	Deserialize(*value);
}

void XMLDeserializer::DeserializeArray(bool * const values, uint32_t itemCount) 
{
}
void XMLDeserializer::DeserializeArray(char * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(wchar_t * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(float * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(double * const values, uint32_t itemCount)
{
}

void XMLDeserializer::DeserializeArray(int8_t * const values, uint32_t itemCount) 
{
}
void XMLDeserializer::DeserializeArray(int16_t * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(int32_t * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(int64_t * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(uint8_t * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(uint16_t * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(uint32_t * const values, uint32_t itemCount)
{
}
void XMLDeserializer::DeserializeArray(uint64_t * const values, uint32_t itemCount)
{
}

void XMLDeserializer::DeserializeArray(std::string * const values, uint32_t itemCount) 
{
}
void XMLDeserializer::DeserializeArray(std::wstring * const values, uint32_t itemCount)
{
}

void XMLDeserializer::DeserializeArray(ObjectBase * const values, uint32_t itemCount)
{
}

} // namespace  Introspection

#pragma warning(pop)
