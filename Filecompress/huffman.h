#include<iostream>
#include<windows.h>
#include"heap.h"
using namespace std;


template<class W>
struct HuffmanTreeNode
{
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;
	W _w; // 权值

	HuffmanTreeNode(const W& w)
		:_w(w)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};


template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:_root(NULL)
	{}

	HuffmanTree(W* a, size_t n, const W& invalid)
	{
		// 内部类
		struct NodeCompareX
		{
			bool operator()(const Node* left, const Node* right) const
			{
				return left->_w < right->_w;
			}
		};

		Heap<Node*, NodeCompareX> minHeap;
		//创建哈夫曼树
		for (size_t i = 0; i < n; ++i)
		{
			if (a[i] != invalid)
			{
				minHeap.Push(new Node(a[i]));
			}
			
		}		
		while (minHeap._size() > 1)
		{
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node* right = minHeap.Top();
			minHeap.Pop();
			Node* parent = new Node(left->_w + right->_w);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;
			minHeap.Push(parent);
		}
		_root = minHeap.Top();
	}

	Node* GetRoot()
	{
		return _root;
	}
	size_t size(Node* root)
	{
		if (root == NULL)
		{
			return 0;
		}
		size_t i = size(root->_left);
		size_t j = size(root->_right);
		return i + j + 1;
	}

protected:
	Node* _root;
};




