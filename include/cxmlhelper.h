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
  void setName(const char* n) {strncpy(name,n,sizeof (name)); }
  void setVal(const char* v) {strncpy(value,v,sizeof (value)); }
};


typedef my_vector<stAttr,10> tAttrList;
class cXmlHelper
{
public:
  cXmlHelper();
  void initXml();
  enSdRes openFile(const char* name);
  void closeFile();
  void openTag(const char* tagName, tAttrList* attr = NULL, bool newLine = true, bool close = false, bool withIndent = true);
  void closeTag(const char* tagName);
  void value(const char* value, bool newLine = true);
  void value(double value, bool newLine = true);
  void value(int value, bool newLine = true);
  void simpleTag(const char* tagName, const char* val, tAttrList* attr = NULL);
  void simpleTag(const char* tagName, float val, tAttrList* attr = NULL);
  void simpleTagNoValue(const char* tagName, tAttrList* attr = NULL);

private:
  void newLine(bool nl = true);
  void writeString(const char* str);
  void indent(bool open = false);
  tFILE m_file;
  int m_indent;
  bool m_fileIsOpen;
  bool m_lastWasClose;
  bool m_lastWasOpen;
  bool m_newLine;
};

#endif // CXMLHELPER_H
