#include<iostream>
#include<Windows.h>
#include<vector>
#include<assert.h>
#include<memory>
using namespace std;




//**********************************************************************************************//
//*******************************创建一个堆(大小堆)*********************************************//


template<class T>
struct Less
{
	bool operator()(const T& l, const T& r)const
	{
		return l < r;
	}
};
template<class T>
struct Greator
{
	bool operator()(const T& l, const T& r)const
	{
		return l > r; 
	}
};


template<class T, class Compare>
class Heap
{
public:
	Heap()
	{}
	Heap(T* a, size_t n)
	{
		_a.reserve(n);
		for (size_t i = 0; i < n; ++i)
		{
			_a.push_back(a[i]);
		}

		//建堆
		for (int i = (_a.size() - 2) / 2; i >= 0; --i)
		{
			_AdjustDown(i);
		}
	}

	void Push(const T& x)
	{
		_a.push_back(x);
		_AdjustUp(_a.size() - 1);
	}

	void Pop()
	{
		assert(!_a.empty());
		swap(_a[0], _a[_a.size() - 1]);
		_a.pop_back();
		_AdjustDown(0);
	}

	const T& Top()  const
	{
		return _a[0];
	}
	size_t _size()
	{
		return _a.size();
	}

protected:

	void _AdjustDown(size_t root)
	{
		Compare ptr;
		int parent = root;
		int child = parent * 2 + 1;
		while (child < _a.size())
		{
			if (child + 1 < _a.size() && ptr(_a[child + 1], _a[child]))
			{
				++child;
			}
			if (ptr(_a[child], _a[parent]))
			{
				swap(_a[parent], _a[child]);
				parent = child;
				child = parent * 2 + 1;
			}
			else
			{
				break;
			}
		}
	}
	void _AdjustUp(int child)
	{
		Compare fun;
		int parent = (child - 1) >> 1;
		while (child > 0)
		{
			if (fun(_a[child], _a[parent]))
			{
				swap(_a[child], _a[parent]);
				child = parent;
				parent = (child - 1) >> 1;
			}
			else
			{
				break;
			}
		}
	}



protected:
	vector<T>  _a;
};




//void Test()
//{
//	int a[] = { 1, 2, 3, 4, 5, 6 };
//	Heap<int, Greator<int>>hp1(a, sizeof(a) / sizeof(a[0]));
//	hp1.sort();
//	int i = 0;
//}