#ifndef INTROSPECTION_ISERIALIZER_H
#define INTROSPECTION_ISERIALIZER_H

#pragma once

namespace  Introspection
{
	class ObjectBase;

class ISerializer
{
public:

	virtual void Serialize(bool value) = 0;
	virtual void Serialize(char value) = 0;
	virtual void Serialize(wchar_t value) = 0;
	virtual void Serialize(float value) = 0;
	virtual void Serialize(double value) = 0;

	virtual void Serialize(int8_t value) = 0;
	virtual void Serialize(int16_t value) = 0;
	virtual void Serialize(int32_t value) = 0;
	virtual void Serialize(uint8_t value) = 0;
	virtual void Serialize(uint16_t value) = 0;
	virtual void Serialize(uint32_t value) = 0;
	virtual void Serialize(int64_t value) = 0;
	virtual void Serialize(uint64_t value) = 0;

	virtual void Serialize(std::string const & value) = 0;
	virtual void Serialize(std::wstring const & value) = 0;

	virtual void Serialize(ObjectBase const & object) = 0;

	virtual void Serialize(char const * const value) = 0;
	virtual void Serialize(wchar_t const * const value) = 0;

	virtual void Serialize(bool const * const value) = 0;
	virtual void Serialize(float const * const value) = 0;
	virtual void Serialize(double const * const value) = 0;

	virtual void Serialize(int8_t const * const value) = 0;
	virtual void Serialize(int16_t const * const value) = 0;
	virtual void Serialize(int32_t const * const value) = 0;
	virtual void Serialize(int64_t const * const value) = 0;
	virtual void Serialize(uint8_t const * const value) = 0;
	virtual void Serialize(uint16_t const * const value) = 0;
	virtual void Serialize(uint32_t const * const value) = 0;
	virtual void Serialize(uint64_t const * const value) = 0;

	virtual void Serialize(std::string const * const value) = 0;
	virtual void Serialize(std::wstring const * const value) = 0;

	virtual void Serialize(ObjectBase const * const object) = 0;

	virtual void SerializeArray(bool const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(char const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(wchar_t const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(float const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(double const * const values, uint32_t itemCount) = 0;

	virtual void SerializeArray(int8_t const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(int16_t const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(int32_t const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(int64_t const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(uint8_t const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(uint16_t const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(uint32_t const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(uint64_t const * const values, uint32_t itemCount) = 0;

	virtual void SerializeArray(std::string const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(std::wstring const * const values, uint32_t itemCount) = 0;

	virtual void SerializeArray(ObjectBase const * const values, uint32_t itemCount) = 0;

	virtual void SerializeArray(bool const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(char const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(wchar_t const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(float const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(double const * const * const values, uint32_t itemCount) = 0;

	virtual void SerializeArray(int8_t const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(int16_t const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(int32_t const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(int64_t const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(uint8_t const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(uint16_t const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(uint32_t const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(uint64_t const * const * const values, uint32_t itemCount) = 0;

	virtual void SerializeArray(std::string const * const * const values, uint32_t itemCount) = 0;
	virtual void SerializeArray(std::wstring const * const * const values, uint32_t itemCount) = 0;

	virtual void SerializeArray(ObjectBase const * const * const values, uint32_t itemCount) = 0;


protected:
	ISerializer() {}

	virtual ~ISerializer() {}
};


} // namespace  Introspection
#endif // INTROSPECTION_ISERIALIZER_H