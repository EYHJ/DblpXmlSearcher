#include "XmlItem.h"


//辅助函数，去掉string串中的标签
string XmlItem::throwTag(string&& right) {
	ll head = right.find('<');
	ll tail;
	while(head != string::npos) {
		tail = right.find('>');
		right.erase(head, tail - head + 1);
		head = right.find('<');
	}
	while(right[0] == ' ')
		right.erase(0, 1);
	return right;
}

//辅助函数，判断所给字符是否字母
bool XmlItem::isletter(const char & vchar) {
	if(('A' <= vchar && vchar <= 'Z') || ('a' <= vchar && vchar <= 'z'))
		return true;
	return false;
}

XmlItem::XmlItem(const string& xmlitem):
	tag(), detail() {
	item = xmlitem;
	posAuthor = 0;
	posTag = 2;//初始化为2，防止找到记录头的标签
}

XmlItem::XmlItem(const XmlItem & right):
	tag(right.tag), detail(right.detail) {
	item = right.item;
    posAuthor = right.posAuthor;
    posTag = right.posTag;
}

XmlItem & XmlItem::operator=(const XmlItem & right) {
	item = right.item;
    posAuthor = right.posAuthor;
    posTag = right.posTag;
	tag = right.tag;
	detail = right.detail;
	return *this;
}

//赋值构造函数
XmlItem & XmlItem::operator=(const string & xmlitem) {
	item = xmlitem;
	posAuthor = 0;
	posTag = 2;
	tag = string();
	detail = string();
	return *this;
}

//在记录中查找title标签
string XmlItem::getTitle() {
	ll head = item.find("<title>");
	if(head == string::npos)
		return string();
	return throwTag(item.substr(head + 7, item.find("</title>", head) - head - 7));
}

//在记录中查找author、editor标签
string XmlItem::getAuthor() {
	size_t head = item.find("<author", posAuthor);
	if(head == string::npos) {
		head = item.find("<editor", posAuthor);
		if(head == string::npos)
			return string();
	}
	
	head = item.find('>', head);
	posAuthor = item.find("</", ++head);
	return item.substr(head, posAuthor - head);
}

//迭代标签
bool XmlItem::nextTag() {
	size_t head = item.find('<', posTag);
	if(head == string::npos)
		return false;
	if(item[++head] == '/')
		return false;
	size_t tail;
	for(tail = head; isletter(item[tail]); ++tail);
	tag = item.substr(head, tail - head);
	head = item.find('>', tail);
	tail = item.find("</", ++head);
	detail = throwTag(item.substr(head, tail - head));
	posTag = ++tail;
	return true;
}

//返回当前迭代器的标签名
string XmlItem::getTag() {
	return tag;
}

//返回当前迭代器的标签内容
string XmlItem::getDetail() {
	return detail;
}
