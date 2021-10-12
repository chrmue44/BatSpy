#ifndef CXMLHELPER_H
#define CXMLHELPER_H
#include "my_vector.h"
#include "cSdCard.h"
//#include <string>
//#include <vector>

#define INDENT  2

struct stAttr
{
  char name[40];
  char value[80];
};


typedef my_vector<stAttr,10> tAttrList;
class cXmlHelper
{
public:
  cXmlHelper();
  void initXml();
  enSdRes openFile(const char* name);
  void closeFile();
  void openTag(const char* tagName, tAttrList* attr = NULL, bool close = false);
  void closeTag(const char* tagName);
  void value(const char* value);
  void value(double value);
  void value(int value);
  void simpleTag(const char* tagName, const char* val, tAttrList* attr = NULL);
  void simpleTagNoValue(const char* tagName, tAttrList* attr = NULL);

private:
  void newLine();
  void writeString(const char* str);
  tFILE m_file;
  int m_indent;
  bool m_fileIsOpen;
};

#endif // CXMLHELPER_H
