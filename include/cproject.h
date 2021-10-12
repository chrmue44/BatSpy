#ifndef CPROJECT_H
#define CPROJECT_H

//#include <string>
//#include <vector>
//#include <filesystem>
//namespace fs = std::experimental::filesystem;
#include "my_vector.h"
#include "cxmlhelper.h"

struct stRecord
{
  char wavFile[40];
  char name[40];
};

class cProject
{
public:
  cProject(const char* localPath) : m_localPath(localPath) {}
  int create(const char*);
  int create(int y, int m, int d, int h, int duration, const char*);

protected:
  cProject();

private:
  void openPrjFile(const char* name);
  void closePrjFile();
  void addFile(const char* wavFile, const char* name);

  void exportActDir();
  void copy(const char* src, const char* dstFile, const char* dstPath);
  int m_startY;
  int m_startM;
  int m_startD;
  int m_startH;
  int m_currY;
  int m_currM;
  int m_currD;
  int m_currH;
  int m_counter;
  my_vector<stRecord, 100> m_records;
  const char* m_localPath;
  char m_currPath[80];
  char m_dstPath[80];
  cXmlHelper m_xml;
};

#endif // CEXPORTER_H
