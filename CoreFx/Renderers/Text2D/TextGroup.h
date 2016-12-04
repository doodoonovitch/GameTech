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

	TextGroup();
	~TextGroup();

	void ForEach(std::function<void(std::weak_ptr<TextPage>)> func)
	{
		for (auto o : mTextPageList)
		{
			func(o);
		}
	}


	void AttachPage(std::weak_ptr<TextPage> page);
	void DetachPage(std::weak_ptr<TextPage> page);

	void SetIsVisible(bool visible);

private:

	typedef std::vector<std::weak_ptr<TextPage>> TextPageList;

	TextPageList mTextPageList;
};


} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTGROUP_H