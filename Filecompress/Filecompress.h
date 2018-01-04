#pragma once
#pragma warning(disable:4996)
#include "huffman.h"
#include <algorithm>

typedef long long LongType;

struct CharInfo
{
	char     _ch;	 // 字符
	LongType _count; // 出现次数
	string	 _code;	 // huffman code

	bool operator != (const CharInfo& info) const
	{
		return _count != info._count;
	}

	CharInfo operator+(const CharInfo& info) const
	{
		CharInfo ret;
		ret._count = _count + info._count;
		return ret;
	}

	bool operator<(const CharInfo& info) const
	{
		return _count < info._count;
	}
};

class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;

public:
	FileCompress()
	{
		for (size_t i = 0; i < 256; ++i)
		{
			_infos[i]._ch = i;
			_infos[i]._count = 0;
		}
	}


	//压缩文件
	void Compress(const char* filename)
	{
		assert(filename);
		struct _huffmanInfo
		{
			char _ch;
			LongType _count;
		};


		FILE* fout = fopen(filename, "rb");
		cout << errno<< endl;
		assert(fout);

		// 1.统计字符出现的次数
		char ch = fgetc(fout);
		while (ch != EOF)
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}

		// 2.构建huffman tree

		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);
		
		// 3.构建huffman编码
		GenerateHuffmanCode(tree.GetRoot());

		//4.开始压缩文件
		string  compressFile = filename;
		compressFile += ".huffman";
		//c_str()的用法就是返回一个正规的C字符串的指针，内容和本string相等
		//
		FILE* fIn = fopen(compressFile.c_str(), "wb");
		assert(fIn);
		_huffmanInfo info;
		size_t size;
		for (size_t i = 0; i < 256; i++)
		{
			if (_infos[i]._count)
			{
				info._ch = _infos[i]._ch;
				info._count = _infos[i]._count;
			    size = fwrite(&info, sizeof(_huffmanInfo), 1, fIn);
				assert(size == 1);
			}
		}
		info._count = 0;
		size = fwrite(&info, sizeof(_huffmanInfo), 1, fIn);
		assert(size == 1);
		
		char value = 0;
		int count = 0;
		//让fout放置到文件开始
		fseek(fout, 0, SEEK_SET);//SEEK_SET 开头 SEEK_CUR 当前位置 SEEK_END 文件结尾 
		ch = fgetc(fout);

		while(ch != EOF)
		{
			string& code = _infos[(unsigned char)ch]._code;
			for (size_t i = 0; i < code.size(); ++i)
			{
				value <<= 1;
				if (code[i] == '1')
					value |= 1;
				else
					value |= 0;
				++count;
				if (count == 8)
				{
					fputc(value, fIn);
					value = 0;
					count = 0;
				}
			}
			ch = fgetc(fout);
		}
		if (count != 0)
		{
			value <<= (8 - count);
			fputc(value, fIn);
		}
		fclose(fIn);
		fclose(fout);
	}


	void GenerateHuffmanCode(Node* root)
	{

		if (root == NULL)
		{
			return;
		}
		if (root->_left == NULL &&root->_right == NULL)
		{
			string& code = _infos[(unsigned char)root->_w._ch]._code;
			Node* cur = root;
			Node* parent = cur->_parent;
			while (parent)
			{
				if (parent->_left == cur)
				{
					code.push_back('0');
				}
				if (parent->_right == cur)
				{
					code.push_back('1');
				}
				cur = parent;
				parent = cur->_parent;
			}
			reverse(code.begin(), code.end());
		}
		GenerateHuffmanCode(root->_left);
		GenerateHuffmanCode(root->_right);

	}



	//string code;
	//构建huffman树的非递归
	//void GenerateHuffmanCode(Node* root,string code)
	//{
	//	if (root == NULL)
	//		return;
	//	if (root->_left == NULL && root->_right)
	//	{
	//		_infos[root->_w._ch]._code = code;
	//		return;
	//	}
	//	GenerateHuffmanCode(root->_left,code+'0');
	//	GenerateHuffmanCode(root->_right,code+'1');
	//}


	

	//解压缩文件
	void Uncompress(const char* filename)
	{

		//为了区分解压出来的文件，不覆盖原文件名.
		assert(filename);
		struct _huffmanInfo
		{
			char _ch;
			LongType _count;
		};
		string uncompressFile = filename;
		//让pos等于找到第一个'.'出现的的下标位置
		size_t pos = uncompressFile.rfind('.');
		assert(pos != string::npos);
		//从0开始取pos个字符
		uncompressFile = uncompressFile.substr(0, pos);
		uncompressFile += ".unhuffman";
		FILE* fIn = fopen(uncompressFile.c_str(), "wb");
		size_t size;
		assert(fIn);
		FILE* fout = fopen(filename, "rb");

		while (1)
		{
			_huffmanInfo info;
			size = fread(&info, sizeof(_huffmanInfo), 1, fout);
			assert(1 == size);
			if (info._count)
			{
				_infos[(unsigned char)info._ch]._ch = info._ch;
				_infos[(unsigned char)info._ch]._count = info._count;
			}
			else

				break;
		}

		//重新构建huffman树.
		CharInfo invalid;
	    invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);
		
		//解压缩
		Node* root = tree.GetRoot();
		GenerateHuffmanCode(tree.GetRoot());
		LongType charcount = root->_w._count;

		char value;
		value = fgetc(fout);
		Node* cur = root;
		int count = 0;
		while (charcount)
		{
			for (int pos = 7; pos >= 0; --pos)
			{
				if (value & (1 << pos)) //1
					cur = cur->_right;
				else                    //0
					cur = cur->_left;
				if (cur->_left == NULL && cur->_right == NULL)
				{
					fputc(cur->_w._ch, fIn);
					cur = root;
					--charcount;
					if (charcount == 0)
					{
						break;
					}
				}
			}
			value = fgetc(fout);
		}
		fclose(fIn);
		fclose(fout);
	}


protected:
	CharInfo _infos[256];
};


//这里是压缩文字
void TestFileCompress()
{
	FileCompress fc;
	fc.Compress("liangliang.txt");
}
void TestFileCompress2()
{
	FileCompress fc;
	fc.Uncompress("liangliang.txt.huffman");
}


////这里是压缩图片
//void TestFileCompress()
//{
//	FileCompress fc;
//	fc.Compress("liangliang.txt");
//}
//
//void TestFileCompress2()
//{
//	FileCompress fc;
//	fc.Uncompress("liangliang.huffman");
//}