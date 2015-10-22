#ifndef INTROSPECTION_ISERIALIZER_H
#define INTROSPECTION_ISERIALIZER_H

#pragma once

namespace  Introspection
{
	class TypeInfo;

class ISerializer
{
public:

	virtual bool SerializeBasicType(int8_t value) = 0;
	virtual bool SerializeBasicType(int16_t value) = 0;
	virtual bool SerializeBasicType(int32_t value) = 0;
	virtual bool SerializeBasicType(int64_t value) = 0;
	virtual bool SerializeBasicType(uint8_t value) = 0;
	virtual bool SerializeBasicType(uint16_t value) = 0;
	virtual bool SerializeBasicType(uint32_t value) = 0;
	virtual bool SerializeBasicType(uint64_t value) = 0;

	virtual bool SerializeBasicType(const int8_t * value) = 0;
	virtual bool SerializeBasicType(const int16_t * value) = 0;
	virtual bool SerializeBasicType(const int32_t * value) = 0;
	virtual bool SerializeBasicType(const int64_t * value) = 0;
	virtual bool SerializeBasicType(const uint8_t * value) = 0;
	virtual bool SerializeBasicType(const uint16_t * value) = 0;
	virtual bool SerializeBasicType(const uint32_t * value) = 0;
	virtual bool SerializeBasicType(const uint64_t * value) = 0;
	
	virtual bool SerializeBasicType(bool value) = 0;
	virtual bool SerializeBasicType(float value) = 0;
	virtual bool SerializeBasicType(double value) = 0;

	virtual bool SerializeBasicType(const bool * value) = 0;
	virtual bool SerializeBasicType(const float * value) = 0;
	virtual bool SerializeBasicType(const double * value) = 0;

	virtual bool SerializeBasicType(const std::string& value) = 0;
	virtual bool SerializeBasicType(const std::wstring& value) = 0;

	virtual bool SerializeBasicType(char value) = 0;
	virtual bool SerializeBasicType(const char* value) = 0;

	virtual bool SerializeBasicType(wchar_t value) = 0;
	virtual bool SerializeBasicType(const wchar_t* value) = 0;

	virtual bool SerializeObject(const TypeInfo* typeInfo, const void* object) = 0;

	virtual ~ISerializer() {}

protected:
	ISerializer() {}
};


} // namespace  Introspection
#endif // INTROSPECTION_ISERIALIZER_H