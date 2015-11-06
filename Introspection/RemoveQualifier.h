#ifndef INTROSPECTION_REMOVEQUALIFIER_H
#define INTROSPECTION_REMOVEQUALIFIER_H

#pragma once

namespace Introspection
{




template <typename T>
struct RemoveQualifier
{
	typedef typename std::remove_cv<T>::type type;
};



} // namespace  Introspection
#endif // INTROSPECTION_REMOVEQUALIFIER_H