#include "cnotes.h"
#include "cSdCard.h"

cnotes::cnotes()
{

}

void cnotes::initNotes(char* fName, thText tHandleStart, thText tHandleEnd)
{
  m_fName = fName;
  cSdCard& sd = cSdCard::inst();
  if(!sd.fileExists(fName))
    createFile(tHandleStart, tHandleEnd);

  tFILE file;
  char buf[64];
  size_t bytesRead;
  enSdRes res = sd.openFile(fName, file, enMode::WRITE);
  if(res == enSdRes::OK)
  {
    for(;;)
    {
      enSdRes res = sd.readLine(file, buf, sizeof(buf), bytesRead);
      if(res != enSdRes::OK)
        break;
      m_count++;
    }
  }
}

void cnotes::getNote(int i, char* buf, size_t bufSize)
{
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  int count = 0;
  size_t bytesRead = 0;
  enSdRes res = sd.openFile(m_fName, file, enMode::READ);
  if(res == enSdRes::OK)
  {
    for(;;)
    {
      enSdRes res = sd.readLine(file, buf, bufSize, bytesRead);
      if(res == enSdRes::OK)
      {
        count++;
        if(count == i)
          break;
        else
          buf[0] = 0;
      }
      else
          buf[0] = 0;
        ;
    }
  }
}

void cnotes::createFile(thText tHandleStart, thText tHandleEnd)
{
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  enSdRes res = sd.openFile(m_fName, file, enMode::WRITE);
  if(res == enSdRes::OK)
  {
    size_t written;
    for(thText t = tHandleStart; t <= tHandleEnd; t++)
    {
      const char* p = Txt::get(t);
      sd.writeFile(file, p, written, strlen(p));
      sd.writeFile(file, "\n", written, strlen("\n"));
    }
    sd.closeFile(file);
  }
}

