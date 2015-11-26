#ifndef INTROSPECTION_XMLSERIALIZER_H
#define INTROSPECTION_XMLSERIALIZER_H

#pragma once

namespace  Introspection
{



class XMLSerialization : public ISerializer
{
public:

	XMLSerialization(tinyxml2::XMLDocument& xmldoc, tinyxml2::XMLNode* rootNode = nullptr);

	~XMLSerialization();

	virtual void Serialize(bool value) override;
	virtual void Serialize(char value) override;
	virtual void Serialize(wchar_t value) override;
	virtual void Serialize(float value) override;
	virtual void Serialize(double value) override;

	virtual void Serialize(int8_t value) override;
	virtual void Serialize(int16_t value) override;
	virtual void Serialize(int32_t value) override;
	virtual void Serialize(uint8_t value) override;
	virtual void Serialize(uint16_t value) override;
	virtual void Serialize(uint32_t value) override;
	virtual void Serialize(int64_t value) override;
	virtual void Serialize(uint64_t value) override;

	virtual void Serialize(std::string const & value) override;
	virtual void Serialize(std::wstring const & value) override;

	virtual void Serialize(ObjectBase const & object) override;

	virtual void Serialize(char const * const value) override;
	virtual void Serialize(wchar_t const * const value) override;

	virtual void Serialize(bool const * const value) override;
	virtual void Serialize(float const * const value) override;
	virtual void Serialize(double const * const value) override;

	virtual void Serialize(int8_t const * const value) override;
	virtual void Serialize(int16_t const * const value) override;
	virtual void Serialize(int32_t const * const value) override;
	virtual void Serialize(int64_t const * const value) override;
	virtual void Serialize(uint8_t const * const value) override;
	virtual void Serialize(uint16_t const * const value) override;
	virtual void Serialize(uint32_t const * const value) override;
	virtual void Serialize(uint64_t const * const value) override;

	virtual void Serialize(std::string const * const value) override;
	virtual void Serialize(std::wstring const * const value) override;

	virtual void Serialize(ObjectBase const * const object) override;

	virtual void SerializeArray(bool const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(char const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(wchar_t const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(float const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(double const * const values, uint32_t itemCount) override;

	virtual void SerializeArray(int8_t const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(int16_t const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(int32_t const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(int64_t const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(uint8_t const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(uint16_t const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(uint32_t const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(uint64_t const * const values, uint32_t itemCount) override;

	virtual void SerializeArray(std::string const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(std::wstring const * const values, uint32_t itemCount) override;

	virtual void SerializeArray(ObjectBase const * const values, uint32_t itemCount) override;

	virtual void SerializeArray(bool const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(char const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(wchar_t const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(float const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(double const * const * const values, uint32_t itemCount) override;

	virtual void SerializeArray(int8_t const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(int16_t const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(int32_t const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(int64_t const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(uint8_t const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(uint16_t const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(uint32_t const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(uint64_t const * const * const values, uint32_t itemCount) override;

	virtual void SerializeArray(std::string const * const * const values, uint32_t itemCount) override;
	virtual void SerializeArray(std::wstring const * const * const values, uint32_t itemCount) override;

	virtual void SerializeArray(ObjectBase const * const * const values, uint32_t itemCount) override;

protected:

	void SetClassAttributes(tinyxml2::XMLElement* elem, const TypeInfo* ti);
	void SetMemberAttributes(tinyxml2::XMLElement* elem, const MemberInfo* mi);

	template<typename T>
	void SetClassAttribute(tinyxml2::XMLElement* elem)
	{
		const TypeInfo* ti = TYPEINFO_TYPE(T);
		SetClassAttributes(elem, ti);
	}

	void SerializeNull()
	{
		_retElem = _xmldoc.NewElement("Null");
	}

	template<typename TValue>
	void InternalSerialize(TValue value)
	{
		std::string s;
		Converter<TValue>::ToString(s, value);
		_retElem = _xmldoc.NewElement("Value");
		_retElem->SetText(s.c_str());
		SetClassAttribute<TValue>(_retElem);
	}

	template<typename TValue>
	void InternalSerializePointer(TValue const * const value)
	{
		if (value == nullptr)
			SerializeNull();
		else
			Serialize(*value);
	}

	template<typename TValue>
	void InternalSerializeArray(TValue const * const values, uint32_t itemCount)
	{
		assert(values != nullptr);

		XMLElement * elem = _xmldoc.NewElement("Array");
		SetClassAttribute<TValue>(elem);
		elem->SetAttribute("ItemCount", itemCount);

		for (uint32_t i = 0; i < itemCount; ++i)
		{
			_retElem = nullptr;

			Serialize(values[i]);

			if (_retElem != nullptr)
			{
				_retElem->SetAttribute("Index", i);
				_retElem->SetName("Item");
				elem->InsertEndChild(_retElem);
			}
		}

		_retElem = elem;
	}

	template<typename TValue>
	void InternalSerializePointerArray(TValue const * const * const values, uint32_t itemCount)
	{
		assert(values != nullptr);
		
		XMLElement * elem = _xmldoc.NewElement("Array");
		SetClassAttribute<TValue>(elem);
		elem->SetAttribute("ItemCount", itemCount);

		for (uint32_t i = 0; i < itemCount; ++i)
		{
			_retElem = nullptr;

			auto p = values[i];
			if (p == nullptr)
				SerializeNull();
			else
				Serialize(*p);

			if (_retElem != nullptr)
			{
				_retElem->SetAttribute("Index", i);
				_retElem->SetName("Item");
				elem->InsertEndChild(_retElem);
			}
		}

		_retElem = elem;
	}

	void SerializeObjectMembers(tinyxml2::XMLElement * node, ObjectBase const & object, const TypeInfo* typeInfo);

protected:

	typedef std::stack<tinyxml2::XMLElement*> XMLNodeStack;

	tinyxml2::XMLDocument& _xmldoc;
	tinyxml2::XMLNode* _rootNode;
	tinyxml2::XMLElement* _retElem;
};

} // namespace  Introspection
#endif // INTROSPECTION_XMLSERIALIZER_H