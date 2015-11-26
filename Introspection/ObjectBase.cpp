#include "stdafx.h"
#include "Introspection.h"


namespace Introspection
{

ObjectBase::ObjectBase()
{
}


ObjectBase::~ObjectBase()
{
}

DEFINE_TYPEINFO_OBJECTBASE(ObjectBase)
{

}

bool ObjectBase::IsKindOf(const ObjectBase* parent) const
{
	return this->GetTypeInfo()->IsKindOf(parent->GetTypeId());
}

void ObjectBase::SerializeMember(ISerializer& serializer, const MemberInfo* mi) const
{
	assert(mi != nullptr);
	mi->GetTypeInfo()->Serialize(&serializer, mi->GetPtr(this), mi->GetIsPointer(), mi->GetExtent());
}

void ObjectBase::DeserializeMember(IDeserializer& serializer, const MemberInfo* mi) const
{
	assert(mi != nullptr);
	mi->GetTypeInfo()->Deserialize(&serializer, mi->GetPtr(this), mi->GetIsPointer(), mi->GetExtent());
}

} // namespace  Introspection
