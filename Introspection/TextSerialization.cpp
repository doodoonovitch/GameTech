#include "stdafx.h"
#include "Introspection.h"


namespace  Introspection
{



TextSerializerHelper::TextSerializerHelper()
	: _level(0)
{

}

TextSerializerHelper::~TextSerializerHelper()
{

}

void TextSerializerHelper::Padding(std::ostream& os)
{
	for (size_t i = 0; i < _level; ++i)
	{
		os << "\t";
	}
}



} // namespace  Introspection
