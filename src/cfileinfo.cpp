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
  snprintf(valbuf,sizeof(valbuf),"%i %s", val, unit);
  strcat(buf,valbuf);
  strcat(buf,"</");
  strcat(buf,tag);
  strcat(buf,">");
  return writeLine(buf);
}

int cFileInfo::getNextChar()
{
    char c;
    size_t bytesRead;
    enSdRes ret = cSdCard::inst().readFile(m_file, &c, bytesRead, 1);
    if((bytesRead == 0) || (ret != enSdRes::OK))
      return -1;
    else
      return c;
}

void cFileInfo::putBack()
{
  size_t pos = cSdCard::inst().getFilePos(m_file);
  if(pos > 0)
  cSdCard::inst().setFilePos(m_file, pos - 1);
}


enToken cFileInfo::getToken() {
  int ret = getNextChar();
  size_t idx = 0;
  if (ret < 0)
    return enToken::END;

  if(ret == '<') {
    ret = getNextChar();
    if(ret < 0)
      return  enToken::END;

    // close tag
    if(ret == '/') {
        do {
          ret = getNextChar();
          m_name[idx++] = (char)ret;
          if(idx >= sizeof (m_name))
            return enToken::ERR_TOKEN;
        } while ((ret != '>') || (ret < 0));
        if((ret > 0) && (idx > 0))
          m_name[idx - 1] = 0;
        if(ret > 0)
          putBack();
        return enToken::CLOSE_TAG;
    }

    //open tag
    else {
      m_name[idx++] = (char)ret;
      do {
        ret = getNextChar();
        m_name[idx++] = (char)ret;
        if(idx >= sizeof (m_name))
          return enToken::ERR_TOKEN;
      } while ((ret != '>') || (ret < 0));
      if((ret > 0) && (idx > 0))
        m_name[idx - 1] = 0;
      if(ret > 0)
        putBack();
      return enToken::OPEN_TAG;
    }
  }

  // text
  else {
      do {
        ret = getNextChar();
        m_name[idx++] = (char)ret;
        if((idx >= sizeof (m_name)) || (ret < 0))
          return enToken::ERR_TOKEN;
      } while (ret != '<');
      if(idx > 0)
        m_name[idx - 1] = 0;
      if(ret > 0)
        putBack();
      return enToken::TEXT;
  }
  return enToken::ERR_TOKEN;
}

