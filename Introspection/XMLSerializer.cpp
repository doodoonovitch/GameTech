#include "stdafx.h"
#include "Introspection.h"


namespace  Introspection
{

using namespace tinyxml2;


XMLSerialization::XMLSerialization(tinyxml2::XMLDocument& xmldoc, tinyxml2::XMLNode* rootNode)
	: _xmldoc(xmldoc)
	, _rootNode(rootNode)
{
}

XMLSerialization::~XMLSerialization()
{

}



void XMLSerialization::Serialize(ObjectBase const & object)
{
	const TypeInfo* typeInfo = object.GetTypeInfo();
	assert(typeInfo != nullptr);
	
	XMLElement * elem = _xmldoc.NewElement("Object");

	if (_rootNode == nullptr)
	{
		_xmldoc.InsertFirstChild(elem);
	}
	else
	{
		_rootNode->InsertEndChild(elem);
	}

	SetClassAttributes(elem, typeInfo);
	
	SerializeObjectMembers(elem, object, typeInfo);

	_retElem = elem;
}

void XMLSerialization::SetClassAttributes(tinyxml2::XMLElement* elem, const TypeInfo* ti)
{
	elem->SetAttribute("Class", ti->GetTypeName().c_str());
}

void XMLSerialization::SetMemberAttributes(tinyxml2::XMLElement* elem, const MemberInfo* mi)
{
	elem->SetAttribute("Name", mi->GetName().c_str());
	elem->SetAttribute("IsPointer", mi->GetIsPointer());
}

void XMLSerialization::SerializeObjectMembers(XMLElement * elem, ObjectBase const & object, const TypeInfo* typeInfo)
{
	const TypeInfo* parent = typeInfo->GetParent();
	if (parent != nullptr)
		SerializeObjectMembers(elem, object, parent);

	for (auto mi : typeInfo->GetMembers())
	{
		if (mi->GetSerializable())
		{
			_retElem = nullptr;

			object.SerializeMember(*this, mi);

			if (_retElem != nullptr)
			{
				_retElem->SetName("Member");
				SetMemberAttributes(_retElem, mi);
				elem->InsertEndChild(_retElem);
			}
		}
	}
}

void XMLSerialization::Serialize(bool value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(char value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(wchar_t value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(float value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(double value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(int8_t value) 
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(int16_t value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(int32_t value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(int64_t value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(uint8_t value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(uint16_t value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(uint32_t value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(uint64_t value)
{
	InternalSerialize(value);
}

void XMLSerialization::Serialize(std::string const & value)
{
	InternalSerialize(value.c_str());
}

void XMLSerialization::Serialize(std::wstring const & value)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string s = converter.to_bytes(value);
	InternalSerialize(s.c_str());
}


void XMLSerialization::Serialize(bool const * const values)
{
	InternalSerialize(values);
}

void XMLSerialization::Serialize(char const * const value)
{
	InternalSerialize<char*>((char*)value);
}

void XMLSerialization::Serialize(wchar_t const * const value)
{
	InternalSerialize<wchar_t*>((wchar_t*)value);
}

void XMLSerialization::Serialize(float const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(double const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(int8_t const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(int16_t const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(int32_t const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(int64_t const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(uint8_t const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(uint16_t const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(uint32_t const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(uint64_t const * const value)
{
	InternalSerializePointer(value);
}

void XMLSerialization::Serialize(std::string const * const value)
{
	if (value == nullptr)
		return SerializeNull();
	else
		return Serialize(*value);
}

void XMLSerialization::Serialize(std::wstring const * const value)
{
	if (value == nullptr)
		SerializeNull();
	else
		Serialize(*value);
}

void XMLSerialization::Serialize(ObjectBase const * const value)
{
	if (value == nullptr)
		SerializeNull();
	else
		Serialize(*value);
}

void XMLSerialization::SerializeArray(bool const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(char const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(wchar_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(float const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(double const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void XMLSerialization::SerializeArray(int8_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(int16_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(int32_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(int64_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(uint8_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(uint16_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(uint32_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(uint64_t const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void XMLSerialization::SerializeArray(std::string const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(std::wstring const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void XMLSerialization::SerializeArray(ObjectBase const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void XMLSerialization::SerializeArray(char const * const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}

void XMLSerialization::SerializeArray(wchar_t const * const * const values, uint32_t itemCount)
{
	InternalSerializeArray(values, itemCount);
}


void XMLSerialization::SerializeArray(bool const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(float const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(double const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}


void XMLSerialization::SerializeArray(int8_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(int16_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(int32_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(int64_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(uint8_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(uint16_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(uint32_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(uint64_t const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}


void XMLSerialization::SerializeArray(std::string const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}

void XMLSerialization::SerializeArray(std::wstring const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}


void XMLSerialization::SerializeArray(ObjectBase const * const * const values, uint32_t itemCount)
{
	InternalSerializePointerArray(values, itemCount);
}





} // namespace  Introspection
