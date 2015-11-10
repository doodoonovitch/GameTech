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

	virtual bool Serialize(bool value) override;
	virtual bool Serialize(char value) override;
	virtual bool Serialize(wchar_t value) override;
	virtual bool Serialize(float value) override;
	virtual bool Serialize(double value) override;

	virtual bool Serialize(int8_t value) override;
	virtual bool Serialize(int16_t value) override;
	virtual bool Serialize(int32_t value) override;
	virtual bool Serialize(uint8_t value) override;
	virtual bool Serialize(uint16_t value) override;
	virtual bool Serialize(uint32_t value) override;
	virtual bool Serialize(int64_t value) override;
	virtual bool Serialize(uint64_t value) override;

	virtual bool Serialize(std::string const & value) override;
	virtual bool Serialize(std::wstring const & value) override;

	virtual bool Serialize(ObjectBase const & object) override;

	virtual bool Serialize(char const * const value) override;
	virtual bool Serialize(wchar_t const * const value) override;

	virtual bool Serialize(bool const * const value) override;
	virtual bool Serialize(float const * const value) override;
	virtual bool Serialize(double const * const value) override;

	virtual bool Serialize(int8_t const * const value) override;
	virtual bool Serialize(int16_t const * const value) override;
	virtual bool Serialize(int32_t const * const value) override;
	virtual bool Serialize(int64_t const * const value) override;
	virtual bool Serialize(uint8_t const * const value) override;
	virtual bool Serialize(uint16_t const * const value) override;
	virtual bool Serialize(uint32_t const * const value) override;
	virtual bool Serialize(uint64_t const * const value) override;

	virtual bool Serialize(std::string const * const value) override;
	virtual bool Serialize(std::wstring const * const value) override;

	virtual bool Serialize(ObjectBase const * const object) override;

	virtual bool SerializeArray(bool const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(char const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(wchar_t const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(float const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(double const * const values, uint32_t itemCount) override;

	virtual bool SerializeArray(int8_t const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(int16_t const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(int32_t const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(int64_t const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(uint8_t const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(uint16_t const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(uint32_t const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(uint64_t const * const values, uint32_t itemCount) override;

	virtual bool SerializeArray(std::string const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(std::wstring const * const values, uint32_t itemCount) override;

	virtual bool SerializeArray(ObjectBase const * const values, uint32_t itemCount) override;

	virtual bool SerializeArray(bool const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(char const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(wchar_t const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(float const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(double const * const * const values, uint32_t itemCount) override;

	virtual bool SerializeArray(int8_t const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(int16_t const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(int32_t const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(int64_t const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(uint8_t const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(uint16_t const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(uint32_t const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(uint64_t const * const * const values, uint32_t itemCount) override;

	virtual bool SerializeArray(std::string const * const * const values, uint32_t itemCount) override;
	virtual bool SerializeArray(std::wstring const * const * const values, uint32_t itemCount) override;

	virtual bool SerializeArray(ObjectBase const * const * const values, uint32_t itemCount) override;

protected:

	bool SerializeNull()
	{
		_stream << L"null ";
		return true;
	}

	template<typename TValue>
	bool InternalSerialize(TValue value)
	{
		_stream << value << L" ";
		return true;
	}

	template<typename TValue>
	bool InternalSerializePointer(TValue const * const value)
	{
		if (value == nullptr)
			return SerializeNull();
		else
			return InternalSerialize(*value);
	}

	template<typename TValue>
	bool InternalSerializeArray(TValue const * const values, uint32_t itemCount)
	{
		if (values == nullptr)
			return SerializeNull();

		_stream << L"[ " ;
		for (uint32_t i = 0; i < itemCount; ++i)
		{
			if (i > 0)
				_stream << L", ";
			Serialize(values[i]);
		}
		_stream << L"] ";

		return true;
	}

	template<typename TValue>
	bool InternalSerializePointerArray(TValue const * const * const values, uint32_t itemCount)
	{
		if (values == nullptr)
			return SerializeNull();

		_stream << L"[ ";
		for (uint32_t i = 0; i < itemCount; ++i)
		{
			if (i > 0)
				_stream << L", ";
			auto p = values[i];
			if (p == nullptr)
				SerializeNull();
			else
				Serialize(*p);
		}
		_stream << L"] ";

		return true;
	}

	bool SerializeObjectMembers(ObjectBase const & object, const TypeInfo* typeInfo, size_t& memberCount) ;

protected:

	std::wostream& _stream;

};

} // namespace  Introspection
#endif // INTROSPECTION_TEXTSERIALIZER_H