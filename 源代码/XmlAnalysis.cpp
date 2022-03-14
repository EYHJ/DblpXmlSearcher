#include "XmlAnalysis.h"
//#include<QDir>

XmlAnalysis::XmlAnalysis(const string & filename, const EndlType& endltype):
	xmlPath(filename) {
	anlsPb = 0;
	if(endltype == EndlType::Unix)
		needFix = false;
	else
		needFix = true;
    finished=false;
    mkIndexPath();
    mkdir();

	fstream file(filename, std::ios::in);
	size = file.seekg(0, std::ios::end).tellg();
	file.close();
}

//将标题和对应位置存入容器
void XmlAnalysis::toTitleIndex(const string & bufto, const size_t & pos) {
    auto head = bufto.find("<title>");
	if(head == string::npos)
		return;
	string title = throwTag(bufto.substr(head + 7, bufto.find("</title>", head) - head - 7));
	titleIndex.insert(std::make_pair(title, pos));
	toYearIndex(bufto, title);//将标题存入对应的年份容器中
}

//将作者和对应位置存入容器
void XmlAnalysis::toAuthorIndex(const string & bufto, const size_t & pos) {
    auto phead = bufto.find("<author");
	string athTag("<author"), athStr;
	if(phead == string::npos) {//若找不到author项，则查找editor项
		athTag = "<editor";
		phead = bufto.find(athTag);
	}

	while(phead != string::npos) {//连续查找全部的作者
		phead = bufto.find('>', phead);
        athStr = bufto.substr(++phead, bufto.find("</", phead) - phead-1);
        std::transform(athStr.begin(), athStr.end(), athStr.begin(), (int(*)(int))std::tolower);

		if(authorIndex.find(athStr) == authorIndex.end())
			authorIndex.insert(std::make_pair(athStr, vector<size_t>()));
		authorIndex[athStr].emplace_back(pos);
		phead = bufto.find(athTag, phead);
	}
}

//将标题存入对应的年份容器中
void XmlAnalysis::toYearIndex(const string & bufto, const string & title) {
    auto pos = bufto.find("<year");
	if(pos == string::npos)
		return;
	string year = bufto.substr(pos + 6, 4);
	if(yearIndex.find(year) == yearIndex.end())
		yearIndex.insert(std::make_pair(year, stringstream()));
	yearIndex[year] << title << std::endl;
}

//保存标题索引文件
void XmlAnalysis::saveTitleIndex() {
	stringstream tbuf;//先写入串流中
	for(auto it = titleIndex.begin(); it != titleIndex.end(); ++it) {
		tbuf << it->first << std::endl << it->second << std::endl;
	}
    fstream tout(indexPath + "/title", std::ios::out);//将串流写入磁盘
	tout << tbuf.str();
    tout.close();
}

//保存作者索引文件
void XmlAnalysis::saveAuthorIndex() {
	stringstream tbuf;//先写入串流中
	vector<size_t>::iterator vit;
	for(auto it = authorIndex.begin(); it != authorIndex.end(); ++it) {
		tbuf << it->first << std::endl;
		for(vit = it->second.begin(); vit != it->second.end(); ++vit) {
			tbuf << *vit << ' ';
		}
		tbuf << std::endl;
	}
	fstream tout(indexPath + "/author", std::ios::out);//将串流写入磁盘
	tout << tbuf.str();
    tout.close();
}

//保存对应年份的标题
void XmlAnalysis::saveYearIndex() {
	string dirPath(indexPath + "/year/");
	string ydPath;
	for(auto it = yearIndex.begin(); it != yearIndex.end(); ++it) {
		ydPath = dirPath + it->first;
		fstream yfile;
		if(isNotFirstWriteYear[it->first]) {//非第一次写入时以追加方式打开文件
			yfile.open(ydPath, std::ios::app);
		}
		else {//第一次写入时建立空文件
			yfile.open(ydPath, std::ios::out);
			isNotFirstWriteYear[it->first] = true;
		}
		yfile << it->second.str();
		it->second.str("");//清空流对象
		it->second.clear();
		yfile.close();
	}
}

//
void XmlAnalysis::mkIndexPath() {
    ll rside = xmlPath.rfind('/');
	if(rside == string::npos)
		rside = -1;
	ll lside = xmlPath.rfind('.');
	if(lside == string::npos)
		lside = xmlPath.size();

    ++rside;
    xmlfileName=xmlPath.substr(rside, lside - rside);
    indexPath =QDir::currentPath().toStdString()+ "/xmlIndex/" + xmlfileName;

}

//建立文件对应的目录
void XmlAnalysis::mkdir() {
    _mkdir("xmlIndex");
    _mkdir(indexPath.c_str());
    _mkdir((indexPath + "/year").c_str());
    //QDir wp;
    //wp.mkdir("xmlIndex");
    //wp.mkdir(indexPath.c_str());
    //wp.mkdir((indexPath + "/year").c_str());
}

//如果换行符是\r\n，则需要修正位置
size_t XmlAnalysis::fixSize(string && right) {
	if(needFix)
		for(size_t pos = right.find('\n'); pos != string::npos; pos = right.find('\n', ++pos))//行计数
			++anlsPb;
	return 0;
}

//如果换行符是\r\n，则需要修正位置，重载版本
void XmlAnalysis::fixSize(const string & right) {
	if(needFix)
		for(size_t pos = right.find('\n'); pos != string::npos; pos = right.find('\n', ++pos))//行计数
			++anlsPb;
}

//私有的复制构造函数，不能使用，会导致文件流错误
XmlAnalysis::XmlAnalysis(const XmlAnalysis & right):
	xmlPath(right.xmlPath), indexPath(right.indexPath),
	titleIndex(right.titleIndex), authorIndex(right.authorIndex), buf(right.buf) {
	anlsPb = right.anlsPb;
	size = right.size;
	needFix = right.needFix;
    finished=right.finished;
}

XmlAnalysis::~XmlAnalysis() {
	clear();
}

void XmlAnalysis::clear() {
	titleIndex.clear();
	authorIndex.clear();
	yearIndex.clear();
	buf.clear();
}

//分析xml文件，并在磁盘上建立索引文件
void XmlAnalysis::analyse(size_t blockSize) {
	fstream file(xmlPath, std::ios::in);
	do {
		vector<char> vchar(blockSize);
		file.read(&vchar[0], blockSize);//读入给定的大小
		string data(&vchar[0], file.gcount());//将数据转到string容器中
		vchar.clear();
		vchar.shrink_to_fit();//清理内存

		data.insert(0, buf);//插入上次处理时未处理的部分
		if(file.gcount() != 0) {//修剪尾部，使结尾是完整的数据记录
			size_t temPos = data.rfind('<', data.rfind("mdate="));
			buf = data.substr(temPos);//多余部分存入缓冲区
			data.erase(temPos);//修剪
		}
		else
			buf.clear();

		string dataBuf;
		//查找记录的标记"mdate="，以其前面的'<'作为记录开始
		for(size_t bmark = data.find("mdate="), head = data.rfind('<', bmark), tail = fixSize(data.substr(0, bmark));
			tail != string::npos; head = tail) {

			//查找下一个"mdate="，以其前面的'<'作为记录结束
			tail = data.find("mdate=", bmark + 1);
			if(tail != string::npos) {
				bmark = tail;
				tail = data.rfind('<', tail);
				dataBuf = data.substr(head, tail - head);
			}
			else
				dataBuf = data.substr(head);

			toTitleIndex(dataBuf, anlsPb + head);//存入标题及位置
			toAuthorIndex(dataBuf, anlsPb + head);//存入作者及位置
			fixSize(dataBuf);//修正换行符导致的位置变化
		}

		saveYearIndex();
		anlsPb += data.size();
	} while(file.gcount() != 0);
	file.close();

	saveTitleIndex();
	saveAuthorIndex();
    anlsPb=size;//将当前位置设置为xml文件的大小，标志分析完成
    finished=true;
}

//返回当前的位置
size_t XmlAnalysis::getPos(){
    return anlsPb;
}

//返回xml文件的大小
size_t XmlAnalysis::getSize(){
    return size;
}

//返回索引文件的路径
string XmlAnalysis::getIndexPath() {
	return indexPath;
}

//返回xml文件的文件名，不包含扩展名
string XmlAnalysis::getXmlName() {
    return xmlfileName;
}

//接受一个string串，返回其去掉所有标签（<tag>）后的string串
string XmlAnalysis::throwTag(string&& right) {
	ll head = right.find('<');
	ll tail;
	while(head != string::npos) {
		tail = right.find('>');
		right.erase(head, tail - head + 1);
		head = right.find('<');
	}
	while(right[0] == ' ')//去掉空白开头
		right.erase(0, 1);
    std::transform(right.begin(), right.end(), right.begin(), (int(*)(int))std::tolower);//转化为小写
	return right;
}
