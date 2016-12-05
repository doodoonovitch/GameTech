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
	DetachPage(page, false);
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

	std::remove_if(mTextPageList.begin(), mTextPageList.end(), [ptr](TextPageWeakPtr a)->bool { return a.lock() == ptr; });

	if (updateRendererState && ptr->GetIsVisible() && mRenderer->GetActiveTextGroup().lock().get() == this)
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
}

bool TextGroup::IsPageAttached(TextPageWeakPtr page) const
{
	auto pagePtr = page.lock();
	if (pagePtr == nullptr)
		return false;

	return IsPageAttached(pagePtr);
}

bool TextGroup::IsPageAttached(std::shared_ptr<TextPage> page) const
{

	auto it = std::find_if(mTextPageList.begin(), mTextPageList.end(), [page](TextPageWeakPtr a)->bool { return a.lock() == page; });
	return !(it == mTextPageList.end());
}


} // namespace Renderers
} // namespace CoreFx
