#ifndef INTROSPECTION_XMLDESERIALIZER_H
#define INTROSPECTION_XMLDESERIALIZER_H

#pragma once

namespace  Introspection
{

class XMLDeserializer :
	public IDeserializer
{
public:
	XMLDeserializer(tinyxml2::XMLNode* rootNode);
	virtual ~XMLDeserializer();

	virtual void Deserialize(ObjectBase & object) override;
	virtual void Deserialize(ObjectBase * & object) override;

	virtual void Deserialize(bool & value) override;
	virtual void Deserialize(char & value) override;
	virtual void Deserialize(wchar_t & value) override;
	virtual void Deserialize(float & value) override;
	virtual void Deserialize(double & value) override;

	virtual void Deserialize(int8_t & value) override;
	virtual void Deserialize(int16_t & value) override;
	virtual void Deserialize(int32_t & value) override;
	virtual void Deserialize(uint8_t & value) override;
	virtual void Deserialize(uint16_t & value) override;
	virtual void Deserialize(uint32_t & value) override;
	virtual void Deserialize(int64_t & value) override;
	virtual void Deserialize(uint64_t & value) override;

	virtual void Deserialize(std::string & value) override;
	virtual void Deserialize(std::wstring & value) override;

	virtual void Deserialize(char * & value) override;
	virtual void Deserialize(wchar_t * & value) override;

	virtual void Deserialize(bool * & value) override;
	virtual void Deserialize(float * & value) override;
	virtual void Deserialize(double * & value) override;

	virtual void Deserialize(int8_t * & value) override;
	virtual void Deserialize(int16_t * & value) override;
	virtual void Deserialize(int32_t * & value) override;
	virtual void Deserialize(int64_t * & value) override;
	virtual void Deserialize(uint8_t * & value) override;
	virtual void Deserialize(uint16_t * & value) override;
	virtual void Deserialize(uint32_t * & value) override;
	virtual void Deserialize(uint64_t * & value) override;

	virtual void Deserialize(std::string * & value) override;
	virtual void Deserialize(std::wstring * & value) override;

	virtual void DeserializeArray(bool * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(char * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(wchar_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(float * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(double * const values, uint32_t itemCount) override;

	virtual void DeserializeArray(int8_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(int16_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(int32_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(int64_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(uint8_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(uint16_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(uint32_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(uint64_t * const values, uint32_t itemCount) override;

	virtual void DeserializeArray(std::string * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(std::wstring * const values, uint32_t itemCount) override;

	virtual void DeserializeArray(ObjectBase * const values, uint32_t itemCount) override;

protected:

	void LogError() {}

	template<typename TValue>
	void BasicTypeDeserialize(TValue& value)
	{
		if (_stack.empty())
			return;

		const char* str = _stack.top()->GetText();
		Converter<TValue>::FromString(value, str);
	}

	typedef std::stack<tinyxml2::XMLElement*> XMLNodeStack;
	XMLNodeStack _stack;
};


} // namespace  Introspection
#endif // INTROSPECTION_XMLDESERIALIZER_H