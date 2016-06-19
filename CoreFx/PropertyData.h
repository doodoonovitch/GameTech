#pragma once
#ifndef COREFX_PROPERTYDATA
#define COREFX_PROPERTYDATA


namespace CoreFx
{


class PropertyData
{
public:

	enum
	{
		Element_Per_Property = 4
	};

	PropertyData(GLuint propertyCount);
	virtual ~PropertyData();

	inline static GLuint GetPropertySize() { return Element_Per_Property * sizeof(GLfloat); }

	inline GLuint GetPropertyCount() const { return mPropertyCount; }

	inline GLuint GetDataSize() const { return mPropertyCount * GetPropertySize(); }

	inline const GLfloat * GetData() const { return mData; }
	inline GLfloat * GetData() { return mData; }

	inline const GLfloat * GetProperty(GLuint propertyIndex) const
	{
		assert(propertyIndex < mPropertyCount);
		return &mData[Element_Per_Property * propertyIndex];
	}

	inline GLfloat * GetProperty(GLuint propertyIndex)
	{
		assert(propertyIndex < mPropertyCount);
		return &mData[Element_Per_Property * propertyIndex];
	}
	
	inline bool GetIsModified() const
	{
		return mIsModified;
	}

	inline void SetIsModified(bool isModified)
	{
		mIsModified = isModified;
	}

	void SetProperty(const GLbitfield values[Element_Per_Property], GLuint propertyIndex);
	void SetProperty(const GLfloat values[Element_Per_Property], GLuint propertyIndex);
	void SetProperty(const glm::vec4 & value, GLuint propertyIndex);
	void SetProperty(const glm::vec3 & value, GLuint propertyIndex);

private:

	GLfloat * mData;
	GLuint mPropertyCount;
	bool mIsModified;

};


} // namespace CoreFx
#endif // COREFX_PROPERTYDATA

