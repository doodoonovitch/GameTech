#ifndef COREFX_SCENEOBJECT_H
#define COREFX_SCENEOBJECT_H

#pragma once

namespace CoreFx
{


	// =======================================================================
	// =======================================================================

	class SceneObject;
	typedef InstanceContainer<SceneObject> SceneObjectContainer;

class SceneObject : public ObjectInstance
{
public:

	SceneObject(SceneObjectType objectType = SceneObjectTypeId::NoLocationObject);

	virtual ~SceneObject();

	inline Frame* GetFrame() const;
	//inline void SetFrame(Frame* frame);
	//inline void SwapFrame(Frame*& frame);

private:

	SceneObject(const SceneObject& rhs) = delete;
	SceneObject& operator=(const SceneObject& rhs) = delete;


protected:

	Frame* mFrame;

};


// =======================================================================
// =======================================================================


inline Frame* SceneObject::GetFrame() const
{
	return mFrame;
}


//inline void SceneObject::SetFrame(Frame* frame)
//{
//	delete mFrame;
//	mFrame = frame;
//}

//inline void SceneObject::SwapFrame(Frame*& frame)
//{
//	std::swap(mFrame, frame);
//}




	// =======================================================================
	// =======================================================================
} // namespace CoreFx

#endif // COREFX_SCENEOBJECT_H