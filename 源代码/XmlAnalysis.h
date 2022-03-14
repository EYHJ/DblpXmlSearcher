#ifndef XMLANALYSIS_H
#define XMLANALYSIS_H
#include<vector>
#include<map>
#include<string>
#include<stdio.h>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<QDir>

using std::vector;
using std::map;
using std::string;
using std::fstream;
using std::stringstream;
using ll = long long;

enum class EndlType {
	Unix, Windows
};

class XmlAnalysis {
	string xmlPath;
	string indexPath;
    string xmlfileName;
	size_t size;
	size_t anlsPb;
	map<string, size_t> titleIndex;
	map<string, vector<size_t> > authorIndex;
	map<string, stringstream> yearIndex;
	map<string, bool> isNotFirstWriteYear;
	string buf;

	void toTitleIndex(const string& bufto, const size_t& pos);
	void toAuthorIndex(const string& bufto, const size_t& pos);
	void toYearIndex(const string& bufto, const string& title);

	void saveTitleIndex();
	void saveAuthorIndex();
	void saveYearIndex();

	void mkIndexPath();
	void mkdir();

	bool needFix;
    bool finished;
	size_t fixSize(string&& right);
	void fixSize(const string& right);
	explicit XmlAnalysis(const XmlAnalysis& right);
	string throwTag(string&& right);

public:
	explicit XmlAnalysis(const string& filename, const EndlType& endltype);
	~XmlAnalysis();
	void clear();

    void analyse(size_t blockSize);
    size_t getPos();
    size_t getSize();
	string getIndexPath();
    string getXmlName();
    bool isDone(){return finished;}
};
#endif
