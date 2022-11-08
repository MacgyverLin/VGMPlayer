#ifndef _Array_h_
#define _Array_h_

#include "vgmdef.h"

template<class T>
class Array
{
public:
	Array(UINT32 size_)
	{
		_m = new T[size_];
		_size = size_;
	}

	~Array()
	{
		delete[] _m;
		_m = 0;
	}

	T& operator[] (UINT32 i)
	{
		assert(i < _size);

		return _m[i];
	}

	const T& operator[] (UINT32 i) const
	{
		assert(i < _size);

		return _m[i];
	}

	UINT32 size() const
	{
		return _size;
	}

	T *_m;
	UINT32 _size;
};

template<class T>
class Vector
{
public:
	Vector(UINT32 size_ = 0)
	{
		if (size_ != 0)
			_m = new T[size_];
		else
			_m = 0;

		_size = size_;
		_capacity = size_;
	}

	~Vector()
	{
		if (_m)
		{
			delete[] _m;
			_m = 0;
		}

		_size = 0;
		_capacity = 0;
	}

	T& operator[] (UINT32 i)
	{
		assert(i < _size);

		return _m[i];
	}

	const T& operator[] (UINT32 i) const
	{
		assert(i < _size);

		return _m[i];
	}

	T& push()
	{
		UINT32 oldSize = _size;
		if (_size <= _capacity || _capacity == 0)
		{
			expand(_size+1);
		}

		return _m[oldSize];
	}

	INT32 find(T& t) const
	{
		for (UINT32 i = 0; i < _size; i++)
		{
			if (_m[i] == t)
			{
				return i;
			}
		}

		return -1;
	}

	void remove(T& t)
	{
		INT32 idx = find(t);
		if (idx != -1)
		{
			if (idx != _size - 1)
			{
				memcpy(&_m[idx], &_m[idx + 1], sizeof(T)*(_size - (idx + 1)));
			}

			_size--;
		}
	}

	T& pop()
	{
		T& rval = _m[_size - 1];

		if (_size != 0)
			shrink(_size - 1);

		return rval;
	}

	void resize(UINT32 newsize_)
	{
		if (newsize_ > _capacity)
		{
			expand(newsize_);
		}
		else if (newsize_ < _capacity)
		{
			shrink(newsize_);
		}
	}

	UINT32 size() const
	{
		return _size;
	}

	UINT32 capacity() const
	{
		return _capacity;
	}
private:
	void expand(UINT32 newsize_)
	{
		T* temp = new T[newsize_];
		memset(temp, 0, sizeof(T) * newsize_);

		if (_m)
		{
			if (_size != 0)
			{
				memcpy(temp, _m, sizeof(T) * _size);
			}

			delete[] _m;
			_m = 0;
		}

		_m = temp;
		_size = newsize_;
		_capacity = newsize_;
	}

	void shrink(UINT32 newsize_)
	{
		_size = newsize_;
	}

	T *_m;
	UINT32 _size;
	UINT32 _capacity;
};

#endif
