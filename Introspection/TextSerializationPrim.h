#ifndef INTROSPECTION_TEXTSERIALIZERPRIM_H
#define INTROSPECTION_TEXTSERIALIZERPRIM_H

#pragma once

namespace  Introspection
{




//template<typename T>
//class TextDeserializerPrim
//{
//	bool Deserialize(std::istream& is, void* object)
//	{
//		is >> *reinterpret_cast<T*>(object);
//	}
//};



template<typename T>
class TextSerializatonPrim : public ISerialization
{
public:

	virtual bool Serialize(std::ostream& os, const void* object) const override
	{
		os << *reinterpret_cast<const T*>(object);
		return true;
	}

	virtual bool Deserialize(std::istream& is, void* object) const override
	{
		//T& v = *reinterpret_cast<T*>(object);
		//is >> v;
		return true;
	}

	static const TextSerializatonPrim<T>* GetInstance()
	{
		static TextSerializatonPrim<T> _instance;
		return &_instance;
	}

private:

	TextSerializatonPrim() {}
	virtual ~TextSerializatonPrim() {}
};



} // namespace  Introspection
#endif // INTROSPECTION_TEXTSERIALIZERPRIM_H