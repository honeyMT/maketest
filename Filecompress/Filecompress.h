#pragma once
#pragma warning(disable:4996)
#include "huffman.h"
#include <algorithm>

typedef long long LongType;

struct CharInfo
{
	char     _ch;	 // �ַ�
	LongType _count; // ���ִ���
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


	//ѹ���ļ�
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

		// 1.ͳ���ַ����ֵĴ���
		char ch = fgetc(fout);
		while (ch != EOF)
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}

		// 2.����huffman tree

		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);
		
		// 3.����huffman����
		GenerateHuffmanCode(tree.GetRoot());

		//4.��ʼѹ���ļ�
		string  compressFile = filename;
		compressFile += ".huffman";
		//c_str()���÷����Ƿ���һ�������C�ַ�����ָ�룬���ݺͱ�string���
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
		//��fout���õ��ļ���ʼ
		fseek(fout, 0, SEEK_SET);//SEEK_SET ��ͷ SEEK_CUR ��ǰλ�� SEEK_END �ļ���β 
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
	//����huffman���ķǵݹ�
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


	

	//��ѹ���ļ�
	void Uncompress(const char* filename)
	{

		//Ϊ�����ֽ�ѹ�������ļ���������ԭ�ļ���.
		assert(filename);
		struct _huffmanInfo
		{
			char _ch;
			LongType _count;
		};
		string uncompressFile = filename;
		//��pos�����ҵ���һ��'.'���ֵĵ��±�λ��
		size_t pos = uncompressFile.rfind('.');
		assert(pos != string::npos);
		//��0��ʼȡpos���ַ�
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

		//���¹���huffman��.
		CharInfo invalid;
	    invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);
		
		//��ѹ��
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


//������ѹ������
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


////������ѹ��ͼƬ
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