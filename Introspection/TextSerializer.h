#ifndef INTROSPECTION_TEXTSERIALIZER_H
#define INTROSPECTION_TEXTSERIALIZER_H

#pragma once

namespace  Introspection
{


class TextSerializerHelper
{
public:

	void Padding(std::wostream& os);
	void IncrementLevel() { ++_level; }
	void DecrementLevel() { --_level; }

	TextSerializerHelper();
	~TextSerializerHelper();

private:

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

	void SerializeNull()
	{
		_stream << L"null ";
	}

	template<typename TValue>
	void InternalSerialize(TValue value)
	{
		_stream << value << L" ";
	}

	template<typename TValue>
	void InternalSerializePointer(TValue const * const value)
	{
		if (value == nullptr)
			SerializeNull();
		else
			InternalSerialize(*value);
	}

	template<typename TValue>
	void InternalSerializeArray(TValue const * const values, uint32_t itemCount)
	{
		if (values == nullptr)
		{
			SerializeNull();
			return;
		}

		_stream << L"[ " ;
		for (uint32_t i = 0; i < itemCount; ++i)
		{
			if (i > 0)
				_stream << L", ";
			Serialize(values[i]);
		}
		_stream << L"] ";
	}

	template<typename TValue>
	void InternalSerializePointerArray(TValue const * const * const values, uint32_t itemCount)
	{
		if (values == nullptr)
		{
			SerializeNull();
			return;
		}

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
	}

	void SerializeObjectMembers(ObjectBase const & object, const TypeInfo* typeInfo, size_t& memberCount) ;

protected:

	std::wostream& _stream;
	TextSerializerHelper _helper;
};

} // namespace  Introspection
#endif // INTROSPECTION_TEXTSERIALIZER_H