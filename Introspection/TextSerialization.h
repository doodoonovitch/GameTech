#ifndef INTROSPECTION_TEXTSERIALIZATION_H
#define INTROSPECTION_TEXTSERIALIZATION_H

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

	void Padding(std::ostream& os);
	void IncrementLevel() { if (_level < std::numeric_limits<size_t>::max()) ++_level; }
	void DecrementLevel() { if (_level > std::numeric_limits<size_t>::min()) --_level; }


private:
	TextSerializerHelper();
	~TextSerializerHelper();

	size_t _level;
};

template<typename T>
class TextSerialization : public ISerialization
{
public:
	~TextSerialization() {}

	virtual bool Serialize(std::ostream& os, const void* object) const override
	{
		const T* obj = reinterpret_cast<const T*>(object);
		TypeInfo* typeInfo = TYPEINFO_TYPE(T);
		assert(typeInfo != nullptr);

		TextSerializerHelper& helper = TextSerializerHelper::Get();
		helper.Padding(os);
		os << "{" << std::endl;
		helper.IncrementLevel();
		for (auto mi : typeInfo->GetMembers())
		{
			if (mi->GetSerializable())
			{
				auto miTypeInfo = mi->GetTypeInfo();
				const ISerialization* ser = miTypeInfo->GetSerialization();
				if (ser != nullptr)
				{
					helper.Padding(os);
					os << mi->GetName() << " : ";
					ser->Serialize(os, mi->GetPtr(obj));
					os << std::endl;
				}
			}
		}
		helper.DecrementLevel();
		os << "}" << std::endl;

		return true;
	}

	virtual bool Deserialize(std::istream& is, void* object) const override
	{
		return false;
	}


	static const TextSerialization<T>* GetInstance()
	{
		static TextSerialization<T> serializer;
		return &serializer;
	}

protected:

	TextSerialization() {}

};

} // namespace  Introspection
#endif // INTROSPECTION_TEXTSERIALIZATION_H