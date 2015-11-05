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

bool ObjectBase::SerializeMember(ISerializer& serializer, const MemberInfo* mi) const
{
	assert(mi != nullptr);
	return mi->GetTypeInfo()->Serialize(&serializer, mi->GetPtr(this), mi->GetIsPointer());
}



} // namespace  Introspection
