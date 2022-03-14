#ifndef XMLITEM_H
#define XMLITEM_H
#include<string>

using std::string;
using ll = long long;

class XmlItem {
	string item;
	size_t posAuthor;
	size_t posTag;
	string tag;
	string detail;

	string throwTag(string&& right);
	bool isletter(const char& vchar);

public:
	XmlItem(const string& xmlitem);
	XmlItem(const XmlItem& right);
	XmlItem& operator= (const XmlItem& right);
	XmlItem& operator= (const string& xmlitem);

	string getTitle();
	string getAuthor();
	bool nextTag();
	string getTag();
	string getDetail();
};
#endif
