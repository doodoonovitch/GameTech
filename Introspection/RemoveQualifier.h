#ifndef INTROSPECTION_REMOVEQUALIFIER_H
#define INTROSPECTION_REMOVEQUALIFIER_H

#pragma once

namespace Introspection
{

template<typename T>
struct isPointer 
{ 
	static const bool value = false; 
};

template<typename T>
struct isPointer<T *> 
{ 
	static const bool value = true; 
};

template<typename T>
struct isPointer<T&> 
{ 
	static const bool value = false; 
};

template<typename T>
struct isArray 
{ 
	static const bool value = false; 
};

template<typename T>
struct isArray<T *> 
{ 
	static const bool value = false; 
};

template<typename T>
struct isArray<T&> 
{ 
	static const bool value = false; 
};



#define INTERNAL_ARRAY_OVERLOAD( x ) \
  template <typename T> \
  struct RemoveQualifier<T[ x ]> \
  { \
    typedef typename RemoveQualifier<T *>::type type; \
  }; \
  \
  template <typename T> \
  struct RemoveQualifier<const T[ x ]> \
  { \
    typedef typename RemoveQualifier<T *>::type type; \
  }; \
  template<typename T> \
  struct isArray<T[ x ]> { static const bool value = true;  }; \
  template<typename T> \
  struct isPointer<T[ x ]> { static const bool value = true;  }

#define ARRAY_OVERLOAD( ) \
  INTERNAL_ARRAY_OVERLOAD( __COUNTER__ )

template <typename T>
struct RemoveQualifier
{
	typedef T type;
};

template <typename T>
struct RemoveQualifier<const T>
{
	typedef typename RemoveQualifier<T>::type type;
};

template <typename T>
struct RemoveQualifier<T&>
{
	typedef typename RemoveQualifier<T>::type type;
};

template <typename T>
struct RemoveQualifier<const T&>
{
	typedef typename RemoveQualifier<T>::type type;
};

template <typename T>
struct RemoveQualifier<T&&>
{
	typedef typename RemoveQualifier<T>::type type;
};

template <typename T>
struct RemoveQualifier<const T *>
{
	typedef typename RemoveQualifier<T *>::type type;
};

// Array literal overloads
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();
ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD(); ARRAY_OVERLOAD();

} // namespace  Introspection
#endif // INTROSPECTION_REMOVEQUALIFIER_H