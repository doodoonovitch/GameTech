#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
namespace Renderers
{


TextGroup::TextGroup(TextRenderer * renderer)
	: mRenderer(renderer)
{
}


TextGroup::~TextGroup()
{
	mTextPageList.clear();
}

void TextGroup::AttachPage(TextPageWeakPtr page)
{
	AttachPage(page, true);
}

void TextGroup::DetachPage(TextPageWeakPtr page)
{
	DetachPage(page, true);
}

void TextGroup::AttachPage(TextPageWeakPtr page, bool updateRendererState)
{
	auto ptr = page.lock();
	if (ptr == nullptr)
		return;

	mTextPageList.push_back(page);

	if (updateRendererState && ptr->GetIsVisible() && mRenderer->GetActiveTextGroup().lock().get() == this)
	{
		mRenderer->InvalidateShaderBuffer();
	}
}

void TextGroup::DetachPage(TextPageWeakPtr page, bool updateRendererState)
{
	auto ptr = page.lock();
	if (ptr == nullptr)
		return;

	int removeCount = 0;
	std::remove_if(mTextPageList.begin(), mTextPageList.end(), [ptr, &removeCount](TextPageWeakPtr a)->bool 
	{ 
		if (a.lock() == ptr)
		{
			++removeCount;
			return true;
		}
		return false; 
	});

	if (updateRendererState && removeCount > 0 && ptr->GetIsVisible() && mRenderer->GetActiveTextGroup().lock().get() == this)
	{
		mRenderer->InvalidateShaderBuffer();
	}
}


void TextGroup::SetIsVisible(bool visible)
{
	ForEach([visible](TextPageWeakPtr page)
	{
		if (auto p = page.lock())
		{
			p->SetIsVisible(visible);
		}
	});

	if (mRenderer->GetActiveTextGroup().lock().get() == this)
	{
		mRenderer->InvalidateShaderBuffer();
	}
}

bool TextGroup::IsPageAttached(TextPageWeakPtr page) const
{
	return IsPageAttached(page.lock().get());
}

bool TextGroup::IsPageAttached(std::shared_ptr<TextPage> page) const
{
	//auto it = std::find_if(mTextPageList.begin(), mTextPageList.end(), [page](TextPageWeakPtr a)->bool { return a.lock() == page; });
	//return !(it == mTextPageList.end());
	return IsPageAttached(page.get());
}

bool TextGroup::IsPageAttached(const TextPage * page) const
{
	if (page == nullptr)
		return false;

	auto it = std::find_if(mTextPageList.begin(), mTextPageList.end(), [page](TextPageWeakPtr a)->bool { return a.lock().get() == page; });
	return !(it == mTextPageList.end());
}


} // namespace Renderers
} // namespace CoreFx
