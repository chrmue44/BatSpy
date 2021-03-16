#include "cfileinfo.h"
#include <cSdCard.h>
#include <cstring>

int cFileInfo::write(const char* fileName, float duration, int32_t sampleRate, const char* date, const char* wavFile)
{
  enSdRes ret = cSdCard::inst().openFile(fileName, m_file, WRITE);
  if(ret == 0) {
    writeLine("<BatRecord>");
    writeTag("FileName", wavFile);
    writeTag("DateTime", date);
    writeTag("Samplerate", sampleRate, "Hz");
    writeTag("Duration", duration, "Sec");
    writeLine("</BatRecord>");
    cSdCard::inst().closeFile(m_file);
  }
  return ret;
}

int cFileInfo::writeLine(const char* text)
{
  size_t bytesWritten;
  size_t len = strlen(text);
  cSdCard::inst().writeFile(m_file, text, bytesWritten, len);
  return cSdCard::inst().writeFile(m_file, "\n", bytesWritten, 1);
}


int cFileInfo::writeTag(const char* tag, const char* text)
{
  char buf[128];
  strcpy(buf, "<");
  strcat(buf,tag);
  strcat(buf,">");
  strcat(buf,text);
  strcat(buf,"</");
  strcat(buf,tag);
  strcat(buf,">");
  return writeLine(buf);
}


int cFileInfo::writeTag(const char* tag, float val, const char* unit)
{
  char buf[128];
  char valbuf[32];
  strcpy(buf,"<");
  strcat(buf,tag);
  strcat(buf,">");
  snprintf(valbuf,sizeof(valbuf),"%f %s", val, unit); 
  strcat(buf,valbuf);
  strcat(buf,"</");
  strcat(buf,tag);
  strcat(buf,">");
  return writeLine(buf);
}

int cFileInfo::writeTag(const char* tag, int32_t val, const char* unit)
{
  char buf[128];
  char valbuf[32];
  strcpy(buf,"<");
strcat(buf,tag);
  strcat(buf,">");
  snprintf(valbuf,sizeof(valbuf),"%li %s", val, unit); 
  strcat(buf,valbuf);
  strcat(buf,"</");
  strcat(buf,tag);
  strcat(buf,">");
  return writeLine(buf);
}
