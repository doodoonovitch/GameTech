#ifndef INTROSPECTION_TYPEINFOMANAGER_H
#define INTROSPECTION_TYPEINFOMANAGER_H

#pragma once




namespace  Introspection
{
	
class TypeInfoManager
{
public:

	static const TypeInfo* Get(const std::string& name)
	{
		return GetInstance()->GetTypeInfoInstance(name);
	}

	static const TypeInfo* Get(const std::wstring& name)
	{
		return GetInstance()->GetTypeInfoInstance(name);
	}

	static void Add(const TypeInfo* typeInfo)
	{
		GetInstance()->AddTypeInfo(typeInfo);
	}

private:

	TypeInfoManager();
	~TypeInfoManager();

	static TypeInfoManager* GetInstance()
	{
		static TypeInfoManager _typeInfoManager;
		return &_typeInfoManager;
	}

	const TypeInfo* GetTypeInfoInstance(const std::string& name) const;
	const TypeInfo* GetTypeInfoInstance(const std::wstring& name) const;
	void AddTypeInfo(const TypeInfo* typeInfo);

private:

	
	std::map<std::string, const TypeInfo*> _typeInfoByName;
	std::map<std::wstring, const TypeInfo*> _typeInfoByWName;
};


} // namespace  Introspection
#endif // INTROSPECTION_TYPEINFOMANAGER_H