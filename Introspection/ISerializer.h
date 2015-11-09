#ifndef INTROSPECTION_ISERIALIZER_H
#define INTROSPECTION_ISERIALIZER_H

#pragma once

namespace  Introspection
{
	class TypeInfo;
	class ObjectBase;

class ISerializer
{
public:

	virtual bool Serialize(bool value) = 0;
	virtual bool Serialize(char value) = 0;
	virtual bool Serialize(wchar_t value) = 0;
	virtual bool Serialize(float value) = 0;
	virtual bool Serialize(double value) = 0;

	virtual bool Serialize(int8_t value) = 0;
	virtual bool Serialize(int16_t value) = 0;
	virtual bool Serialize(int32_t value) = 0;
	virtual bool Serialize(uint8_t value) = 0;
	virtual bool Serialize(uint16_t value) = 0;
	virtual bool Serialize(uint32_t value) = 0;
	virtual bool Serialize(int64_t value) = 0;
	virtual bool Serialize(uint64_t value) = 0;

	virtual bool Serialize(std::string const & value) = 0;
	virtual bool Serialize(std::wstring const & value) = 0;

	virtual bool Serialize(ObjectBase const & object) = 0;

	virtual bool Serialize(char const * const value) = 0;
	virtual bool Serialize(wchar_t const * const value) = 0;

	virtual bool Serialize(bool const * const value) = 0;
	virtual bool Serialize(float const * const value) = 0;
	virtual bool Serialize(double const * const value) = 0;

	virtual bool Serialize(int8_t const * const value) = 0;
	virtual bool Serialize(int16_t const * const value) = 0;
	virtual bool Serialize(int32_t const * const value) = 0;
	virtual bool Serialize(int64_t const * const value) = 0;
	virtual bool Serialize(uint8_t const * const value) = 0;
	virtual bool Serialize(uint16_t const * const value) = 0;
	virtual bool Serialize(uint32_t const * const value) = 0;
	virtual bool Serialize(uint64_t const * const value) = 0;

	virtual bool Serialize(std::string const * const value) = 0;
	virtual bool Serialize(std::wstring const * const value) = 0;

	virtual bool Serialize(ObjectBase const * const object) = 0;

	virtual bool SerializeArray(bool const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(char const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(wchar_t const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(float const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(double const * const values, uint32_t itemCount) = 0;

	virtual bool SerializeArray(int8_t const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(int16_t const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(int32_t const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(int64_t const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(uint8_t const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(uint16_t const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(uint32_t const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(uint64_t const * const values, uint32_t itemCount) = 0;

	virtual bool SerializeArray(std::string const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(std::wstring const * const values, uint32_t itemCount) = 0;

	virtual bool SerializeArray(ObjectBase const * const values, uint32_t itemCount) = 0;

	virtual bool SerializeArray(bool const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(char const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(wchar_t const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(float const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(double const * const * const values, uint32_t itemCount) = 0;

	virtual bool SerializeArray(int8_t const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(int16_t const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(int32_t const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(int64_t const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(uint8_t const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(uint16_t const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(uint32_t const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(uint64_t const * const * const values, uint32_t itemCount) = 0;

	virtual bool SerializeArray(std::string const * const * const values, uint32_t itemCount) = 0;
	virtual bool SerializeArray(std::wstring const * const * const values, uint32_t itemCount) = 0;

	virtual bool SerializeArray(ObjectBase const * const * const values, uint32_t itemCount) = 0;


	virtual ~ISerializer() {}

protected:
	ISerializer() {}
};


} // namespace  Introspection
#endif // INTROSPECTION_ISERIALIZER_H