#pragma once

template<typename T>
class ObjectPool
{
public:
	vector<shared_ptr<T>> mObjects;

public:
    ObjectPool() = default;
    ObjectPool(size_t const size) 
    {
        for (size_t i = 0; i < size; i++) 
            mObjects.emplace_back(make_shared<T>());
    }

    shared_ptr<T> Acquire() 
    {
        if (mObjects.empty())
            return make_shared<T>(); // Create a new object if pool is empty

        shared_ptr<T> obj = mObjects.back();
        mObjects.pop_back();
        return obj;
    }

    void Release(shared_ptr<T> obj) 
    {
        mObjects.push_back(std::move(obj)); // Return object to the pool
    }
};
