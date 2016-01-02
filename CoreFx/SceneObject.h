#ifndef COREFX_SCENEOBJECT_H
#define COREFX_SCENEOBJECT_H

#pragma once

namespace CoreFx
{


	// =======================================================================
	// =======================================================================



class SceneObject
{
public:

	SceneObject(bool hasFrame = false);

	virtual ~SceneObject();

	inline Frame* GetFrame() const;
	inline void SetFrame(Frame* frame);
	inline void SwapFrame(Frame*& frame);

	virtual void Update();
	virtual void Render(glm::mat4 const & VP) = 0;

private:

	SceneObject(const SceneObject& rhs) = delete;
	SceneObject& operator=(const SceneObject& rhs) = delete;


protected:

	Frame* m_pFrame;

};


// =======================================================================
// =======================================================================


inline Frame* SceneObject::GetFrame() const
{
	return m_pFrame;
}


inline void SceneObject::SetFrame(Frame* frame)
{
	delete m_pFrame;
	m_pFrame = frame;
}

inline void SceneObject::SwapFrame(Frame*& frame)
{
	std::swap(m_pFrame, frame);
}




	// =======================================================================
	// =======================================================================
} // namespace CoreFx

#endif // COREFX_SCENEOBJECT_H