#include "XmlIndex.h"

//辅助函数，判断string中是否纯数字
bool XmlIndex::isNum(const string& right) {
    size_t size = right.size();
    char ichar;
    for(unsigned i = 0; i < size; ++i) {
        ichar = right[i];
        if(ichar != ' ')
            if(ichar < '0' || ichar>'9')
                return false;
    }
    return true;
}

//辅助函数，判断字符是否换行符
bool XmlIndex::isLeap(const char & vc) {
    return vc == '\n' || vc == '\r';
}

XmlIndex::XmlIndex(const string & xmlPath, const string & indexPath):
    xml(xmlPath, std::ios::in), index(indexPath, std::ios::in),
    xmlpath(xmlPath), indexpath(indexPath),
    itemList(), pitem(itemList.end()),
    dicKey(), dicVal() {
    bindex = NULL;
    bindexSize = 0;
    bindexSizeR = 0;
    bitem = NULL;
    bitemSize = 0;
    pos = 0;
}

XmlIndex::XmlIndex(const XmlIndex & right):
    xml(right.xmlpath, std::ios::in), index(right.indexpath, std::ios::in),
    xmlpath(right.xmlpath), indexpath(right.indexpath),
    itemList(right.itemList), pitem(itemList.end()),
    dicKey(right.dicKey), dicVal(right.dicVal) {
    setIndexBlockSize(right.bindexSize);
    setItemSize(right.bitemSize);
    pos = right.pos;
}

XmlIndex & XmlIndex::operator=(const XmlIndex & right) {
    xml.close();
    index.close();
    xml.open(right.xmlpath, std::ios::in);
    index.open(right.indexpath, std::ios::in);
    xmlpath = right.xmlpath;
    indexpath = right.indexpath;
    itemList = right.itemList;
    pitem = itemList.end();
    dicKey = right.dicKey;
    dicVal = right.dicVal;
    setIndexBlockSize(right.bindexSize);
    setItemSize(right.bitemSize);
    pos = right.pos;
    return *this;
}

XmlIndex::~XmlIndex() {
    xml.close();
    index.close();
    delete[] bindex;
    delete[] bitem;
}

//设置建立索引的间距大小
void XmlIndex::setIndexBlockSize(size_t blockSize) {
    bindexSize = blockSize;
    bindexSizeR = blockSize * 1.3;//缓冲区大小为间距的1.3倍
    char* nbuf = new char[bindexSizeR + 1];
    nbuf[bindexSizeR] = '\0';//末位设结束符
    delete[] bindex;
    bindex = nbuf;
}

//设置记录的缓冲区大小
void XmlIndex::setItemSize(size_t itemSize) {
    char* nbuf = new char[itemSize + 1];
    nbuf[itemSize] = '\0';//末位设结束符
    if(bitem != NULL) {
        strncpy(nbuf, bitem, itemSize);//复制原内容（若有）
    }
    delete[] bitem;
    bitem = nbuf;
    bitemSize = itemSize;
}

//在内存中建立索引
void XmlIndex::makeIndex() {
    dicKey.clear();
    dicVal.clear();
    dicKey.shrink_to_fit();
    dicVal.shrink_to_fit();//清理容器

    size_t fsize = index.seekg(0, std::ios::end).tellg();
    string buf;
    for(size_t pos = bindexSize; pos < fsize; pos += bindexSize) {//按间距递增
        index.seekg(pos);
        while(getline(index, buf)) {//掠过数字行
            if(isNum(buf))
                break;
        }
        dicVal.emplace_back(index.tellg());
        if(getline(index, buf))
            dicKey.emplace_back(buf);
    }
    int poi=250;
}

//查找函数
bool XmlIndex::find(string key) {
    std::transform(key.begin(), key.end(), key.begin(), (int(*)(int))std::tolower);//将关键字转换为小写
    ll head = 0, mid = 0, tail = ll(dicKey.size()) - 1;
    while(head <= tail) {//二分查找
        mid = (head + tail) / 2;
        auto smg=dicKey[mid].compare(key);
        if(smg>0){
            tail = --mid;
        }
        else if(smg<0){
            head = ++mid;
        }
        else{
            tail = mid;
            head = ++mid;
        }
    }

    index.seekg(tail < 0 ? 0 : dicVal[tail]);
    index.read(bindex, bindexSizeR);
    auto pfind = strstr(bindex, key.c_str());
    if(pfind == NULL)//在块中查找无结果
        return false;
    if(pfind != bindex) {//若查找结果两端不是换行符，则继续查找
        while(!(isLeap(*(pfind - 1)) && isLeap(*(pfind + key.size())))) {
            pfind = strstr(++pfind, key.c_str());
            if(pfind == NULL)
                return false;
        }
    }
    pfind = strstr(pfind, "\n");//读取索引值
    auto pfindl = strstr(++pfind, "\n");
    string bufstr(pfind, pfindl - pfind);
    stringstream buf(bufstr);
    size_t ppos;
    bool work = false;
    itemList.clear();
    while(buf >> ppos) {//装入容器
        itemList.emplace_back(ppos);
        work = true;
    }
    pitem = itemList.begin();//初始化迭代器
    pos=*pitem;
    return work;
}

//迭代索引
bool XmlIndex::next() {
    if(pitem == itemList.end())
        return false;
    pos = *pitem;
    ++pitem;
    return true;
}

//返回当前迭代器的索引
size_t XmlIndex::getPos() {
    return pos;
}

//根据索引获取记录
string XmlIndex::getItem(size_t pos) {
    xml.seekg(pos);
    xml.read(bitem, bitemSize);

    char* fstItem = std::strstr(bitem, "mdate=");
    if(fstItem == NULL)
        return string();
    char* sndItem = std::strstr(fstItem + 1, "mdate=");
    if(sndItem == NULL)
        return string();

    string item(bitem, sndItem - bitem);
    item.erase(item.rfind('<'));
    return item;
}
