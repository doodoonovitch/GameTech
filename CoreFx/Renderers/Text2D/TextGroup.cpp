#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
namespace Renderers
{


TextGroup::TextGroup()
{
}


TextGroup::~TextGroup()
{
	mTextPageList.clear();
}

void TextGroup::AttachPage(std::weak_ptr<TextPage> page)
{
	mTextPageList.push_back(page);
}

void TextGroup::DetachPage(std::weak_ptr<TextPage> page)
{
	std::remove_if(mTextPageList.begin(), mTextPageList.end(), [page](std::weak_ptr<TextPage> a)->bool { return a.lock() == page.lock(); } );
}

void TextGroup::SetIsVisible(bool visible)
{
	ForEach([visible](std::weak_ptr<TextPage> page)
	{
		if (auto p = page.lock())
		{
			p->SetIsVisible(visible);
		}
	});
}


} // namespace Renderers
} // namespace CoreFx
