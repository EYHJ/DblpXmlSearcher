# dblpXmlSearcher

对应的dblpXml文件下载链接为[http://dblp.uni-trier.de/xml/dblp.xml.gz](http://dblp.uni-trier.de/xml/dblp.xml.gz)  
该文件由科学文献的记录序列组成，记录的格式如下所示：  
```xml
<article mdate="2002-01-03" key="persons/Codd71a">  
<author>E. F. Codd</author>  
<title>Further Normalization of the Data Base Relational Model.</title>  
<journal>IBM Research Report, San Jose, California</journal>  
<volume>RJ909</volume>  
<month>August</month>  
<year>1971</year>  
<cdrom>ibmTR/rj909.pdf</cdrom>  
<ee>db/labs/ibm/RJ909.html</ee>  
</article>  
```
每个记录对应一篇文章，其中包含对作者，题名，发表杂志，卷号，出版时间等的详细说明。  

本项目对该xml文件的解析并建立本地title或author的B+树索引，实现高效检索。  
并可以提取其各部分标签及内容。
