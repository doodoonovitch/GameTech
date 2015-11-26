#ifndef INTROSPECTION_IDESERIALIZER_H
#define INTROSPECTION_IDESERIALIZER_H

#pragma once

namespace  Introspection
{
	class TypeInfo;
	class ObjectBase;

class IDeserializer
{
public:

	virtual void Deserialize(bool & value) = 0;
	virtual void Deserialize(char & value) = 0;
	virtual void Deserialize(wchar_t & value) = 0;
	virtual void Deserialize(float & value) = 0;
	virtual void Deserialize(double & value) = 0;

	virtual void Deserialize(int8_t & value) = 0;
	virtual void Deserialize(int16_t & value) = 0;
	virtual void Deserialize(int32_t & value) = 0;
	virtual void Deserialize(uint8_t & value) = 0;
	virtual void Deserialize(uint16_t & value) = 0;
	virtual void Deserialize(uint32_t & value) = 0;
	virtual void Deserialize(int64_t & value) = 0;
	virtual void Deserialize(uint64_t & value) = 0;

	virtual void Deserialize(std::string & value) = 0;
	virtual void Deserialize(std::wstring & value) = 0;

	virtual void Deserialize(ObjectBase & object) = 0;

	virtual void Deserialize(char * & value) = 0;
	virtual void Deserialize(wchar_t * & value) = 0;

	virtual void Deserialize(bool * & value) = 0;
	virtual void Deserialize(float * & value) = 0;
	virtual void Deserialize(double * & value) = 0;

	virtual void Deserialize(int8_t * & value) = 0;
	virtual void Deserialize(int16_t * & value) = 0;
	virtual void Deserialize(int32_t * & value) = 0;
	virtual void Deserialize(int64_t * & value) = 0;
	virtual void Deserialize(uint8_t * & value) = 0;
	virtual void Deserialize(uint16_t * & value) = 0;
	virtual void Deserialize(uint32_t * & value) = 0;
	virtual void Deserialize(uint64_t * & value) = 0;

	virtual void Deserialize(std::string * & value) = 0;
	virtual void Deserialize(std::wstring * & value) = 0;

	virtual void Deserialize(ObjectBase * & object) = 0;

	virtual void DeserializeArray(bool * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(char * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(wchar_t * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(float * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(double * const values, uint32_t itemCount) = 0;

	virtual void DeserializeArray(int8_t * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(int16_t * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(int32_t * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(int64_t * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(uint8_t * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(uint16_t * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(uint32_t * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(uint64_t * const values, uint32_t itemCount) = 0;

	virtual void DeserializeArray(std::string * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(std::wstring * const values, uint32_t itemCount) = 0;

	virtual void DeserializeArray(ObjectBase * const values, uint32_t itemCount) = 0;

	virtual void DeserializeArray(bool * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(char * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(wchar_t * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(float * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(double * * const values, uint32_t itemCount) = 0;

	virtual void DeserializeArray(int8_t * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(int16_t * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(int32_t * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(int64_t * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(uint8_t * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(uint16_t * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(uint32_t * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(uint64_t * * const values, uint32_t itemCount) = 0;

	virtual void DeserializeArray(std::string * * const values, uint32_t itemCount) = 0;
	virtual void DeserializeArray(std::wstring * * const values, uint32_t itemCount) = 0;

	virtual void DeserializeArray(ObjectBase * * const values, uint32_t itemCount) = 0;

protected:

	IDeserializer()
	{
	}

	virtual ~IDeserializer()
	{
	}
};


} // namespace  Introspection
#endif // INTROSPECTION_IDESERIALIZER_H