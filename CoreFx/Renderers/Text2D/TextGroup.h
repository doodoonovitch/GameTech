#pragma once

#ifndef COREFX_RENDERERS_TEXTGROUP_H
#define COREFX_RENDERERS_TEXTGROUP_H


namespace CoreFx
{
namespace Renderers
{

class TextRenderer;

class TextGroup
{
	friend class TextRenderer;

public:

	void ForEach(std::function<void(TextPageWeakPtr)> func)
	{
		TextPageList::iterator it = mTextPageList.begin();
		while (it != mTextPageList.end())
		{
			TextPageWeakPtr pWeakPtr = *it;
			auto page = pWeakPtr.lock();
			if (page == nullptr)
			{
				it = mTextPageList.erase(it);
			}
			else
			{
				++it;
				func(pWeakPtr);
			}
		}
	}


	void AttachPage(TextPageWeakPtr page);
	void DetachPage(TextPageWeakPtr page);
	bool IsPageAttached(TextPageWeakPtr page) const;

	void SetIsVisible(bool visible);

protected:

	TextGroup(TextRenderer * renderer);
	~TextGroup();

	void AttachPage(TextPageWeakPtr page, bool updateRendererState);
	void DetachPage(TextPageWeakPtr page, bool updateRendererState);
	bool IsPageAttached(std::shared_ptr<TextPage> page) const;

private:

	typedef std::list<TextPageWeakPtr> TextPageList;

	TextRenderer * mRenderer;
	TextPageList mTextPageList;
};

typedef std::weak_ptr<TextGroup> TextGroupWeakPtr;


} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTGROUP_H