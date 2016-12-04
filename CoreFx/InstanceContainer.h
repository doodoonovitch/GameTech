#pragma once
#ifndef COREFX_INSTANCECONTAINER_H
#define COREFX_INSTANCECONTAINER_H

namespace CoreFx
{


typedef int32_t InstanceId;

enum
{
	NO_INSTANCEID = (InstanceId)-1L
};



template<typename TObjectInstance>
class InstanceContainer
{
public:

	typedef std::vector<TObjectInstance*> Container;


public:

	InstanceContainer(size_t capacity, size_t pageSize)
		: mPageSize(pageSize)
	{
		mObjs.reserve(capacity);
	}

	~InstanceContainer()
	{
		mObjs.clear();
	}

	size_t GetCapacity() const
	{
		return mObjs.capacity();
	}

	size_t GetCount() const
	{
		return mObjs.size();
	}

	bool Attach(TObjectInstance* obj)
	{
		assert(obj != nullptr);

		if (obj->HasId())
			return false;

		auto count = mObjs.size();
		if (mObjs.capacity() == count)
		{
			mObjs.reserve(count + mPageSize);
		}

		obj->SetInstanceId((InstanceId)count);
		mObjs.push_back(obj);

		return true;
	}

	bool Detach(TObjectInstance* obj)
	{
		assert(obj != nullptr);
		if (!obj->HasId())
			return false;

		if (obj->GetInstanceId() < (InstanceId)(mObjs.size() - 1))
		{
			mObjs[obj->GetInstanceId()] = mObjs.back();
		}

		mObjs.pop_back();
		obj->SetInstanceId(NO_INSTANCEID);

		return true;
	}


	void ForEach(std::function<void(typename TObjectInstance *)> func)
	{
		for (auto o : mObjs)
		{
			func(o);
		}
	}

	void ParallelForEach(std::function<void(typename TObjectInstance *)> func, unsigned int threadCount = 0)
	{
		if (threadCount == 0)
			threadCount = std::thread::hardware_concurrency();

		auto d = GetCount() / threadCount;
		auto r = GetCount() % threadCount;

		std::vector<std::thread> threads;
		threads.reserve(threadCount);

		size_t n = 0;
		for (unsigned int i = 0; i < threadCount; ++i)
		{
			size_t startIndex = n;
			size_t count = d * threadCount;
			if (r > 0)
			{
				++count;
				--r;
			}

			threads.push_back(std::thread([func, this](size_t startIndex, size_t count)
			{
				size_t end = startIndex + count;
				for (size_t i = startIndex; i < end; ++i)
				{
					func(mObjs[i]);
				}
			}, startIndex, count));

			n += count;
		}

		for (std::vector<std::thread>::iterator it = threads.begin(); it != threads.end(); ++it)
		{
			it->join();
		}
	}

	void DetachAll()
	{
		mObjs.clear();
	}

	void DeleteAll()
	{
		Container temp;
		std::swap(temp, mObjs);
		for (auto o : mObjs)
		{
			delete o;
		}
	}

private:

	size_t mPageSize;
	Container mObjs;
};



class ObjectInstance
{
public:

	ObjectInstance()
		: mId(NO_INSTANCEID)
	{
	}

	virtual ~ObjectInstance()
	{
		mId = NO_INSTANCEID;
	}

	InstanceId GetInstanceId() const
	{
		return mId;
	}

	bool HasId() const
	{
		return mId != NO_INSTANCEID;
	}

private:

	void SetInstanceId(InstanceId id)
	{
		mId = id;
	}

private:

	InstanceId mId;

	template<typename T> friend class InstanceContainer;
};



} // namespace CoreFx
#endif // COREFX_INSTANCECONTAINER_H