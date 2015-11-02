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

DEFINE_TYPEINFO(ObjectBase, ObjectBase::_typeid)
{

}

bool ObjectBase::SerializeMember(ISerializer& serializer, const MemberInfo* mi) const
{
	assert(mi != nullptr);
	return mi->GetTypeInfo()->Serialize(&serializer, mi->GetPtr(this));
}


} // namespace  Introspection
