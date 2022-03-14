#ifndef XMLINDEX_H
#define XMLINDEX_H
#include<fstream>
#include<vector>
#include<string>
#include<cstring>
#include<sstream>
#include<algorithm>

using std::fstream;
using std::vector;
using std::string;
using std::stringstream;
using ll = long long;

class XmlIndex {
	fstream xml;
	fstream index;
	string xmlpath;
	string indexpath;

	char* bindex;
	size_t bindexSize;
	size_t bindexSizeR;

	char* bitem;
	size_t bitemSize;
	vector<size_t> itemList;
	vector<size_t>::iterator pitem;
	size_t pos;

	vector<string> dicKey;
	vector<size_t> dicVal;

	bool isNum(const string& right);
    bool isLeap(const char& vc);

public:
	XmlIndex(const string& xmlPath, const string& indexPath);
	XmlIndex(const XmlIndex& right);
	XmlIndex& operator= (const XmlIndex& right);
	~XmlIndex();

	void setIndexBlockSize(size_t blockSize);
	void setItemSize(size_t itemSize);
	void makeIndex();

	bool find(string key);
	bool next();
	size_t getPos();
	string getItem(size_t pos);
};
#endif


