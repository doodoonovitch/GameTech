#ifndef INTROSPECTION_TEXTSERIALIZER_H
#define INTROSPECTION_TEXTSERIALIZER_H

#pragma once

namespace  Introspection
{


class TextSerializerHelper
{
public:

	static TextSerializerHelper& Get()
	{
		static TextSerializerHelper helper;
		return helper;
	}

	void Padding(std::wostream& os);
	void IncrementLevel() { if (_level < std::numeric_limits<size_t>::max()) ++_level; }
	void DecrementLevel() { if (_level > std::numeric_limits<size_t>::min()) --_level; }


private:
	TextSerializerHelper();
	~TextSerializerHelper();

	size_t _level;
};

class TextSerialization : public ISerializer
{
public:

	TextSerialization(std::wostream& stream) : _stream(stream) 
	{
		_stream << std::boolalpha;
	}
	~TextSerialization() {}

	virtual bool SerializeBasicType(int8_t value) override;
	virtual bool SerializeBasicType(int16_t value) override;
	virtual bool SerializeBasicType(int32_t value) override;
	virtual bool SerializeBasicType(int64_t value) override;
	virtual bool SerializeBasicType(uint8_t value) override;
	virtual bool SerializeBasicType(uint16_t value) override;
	virtual bool SerializeBasicType(uint32_t value) override;
	virtual bool SerializeBasicType(uint64_t value) override;

	virtual bool SerializeBasicType(const int8_t * value) override;
	virtual bool SerializeBasicType(const int16_t * value) override;
	virtual bool SerializeBasicType(const int32_t * value) override;
	virtual bool SerializeBasicType(const int64_t * value) override;
	virtual bool SerializeBasicType(const uint8_t * value) override;
	virtual bool SerializeBasicType(const uint16_t * value) override;
	virtual bool SerializeBasicType(const uint32_t * value) override;
	virtual bool SerializeBasicType(const uint64_t * value) override;
	
	virtual bool SerializeBasicType(bool value) override;
	virtual bool SerializeBasicType(float value) override;
	virtual bool SerializeBasicType(double value) override;

	virtual bool SerializeBasicType(const bool * value) override;
	virtual bool SerializeBasicType(const float * value) override;
	virtual bool SerializeBasicType(const double * value) override;

	virtual bool SerializeBasicType(const std::string& value) override;
	virtual bool SerializeBasicType(const std::wstring& value) override;

	virtual bool SerializeBasicType(const std::string* value) override;
	virtual bool SerializeBasicType(const std::wstring* value) override;


	virtual bool SerializeBasicType(char value) override;
	virtual bool SerializeBasicType(const char* value) override;

	virtual bool SerializeBasicType(wchar_t value) override;
	virtual bool SerializeBasicType(const wchar_t* value) override;

	virtual bool SerializeObject(const ObjectBase& object) override;

protected:

	template<typename TValue>
	bool Serialize(TValue value)  
	{
		_stream << value;
		return true;
	}



protected:

	std::wostream& _stream;

};

} // namespace  Introspection
#endif // INTROSPECTION_TEXTSERIALIZER_H