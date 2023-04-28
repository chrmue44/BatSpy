#include "cnotes.h"
#include "cSdCard.h"
#include "cutils.h"
#include "globals.h"

cnotes::cnotes()
{

}

void cnotes::initNotes(const char* fName, thText tHandleStart, thText tHandleEnd)
{
  m_fName = fName;

  char buf[PAR_STR_LEN];
  strncpy(buf, fName, sizeof (buf) - 1);
  strncat(buf, ".", 2);
  strncat(buf, devPars.lang.getActText(), 20);

  cSdCard& sd = cSdCard::inst();
  if(!sd.fileExists(buf))
    createFile(buf, tHandleStart, tHandleEnd);

  tFILE file;
  size_t bytesRead;
  enSdRes res = sd.openFile(buf, file, enMode::READ);
  if(res == enSdRes::OK)
  {
    for(;;)
    {
      enSdRes res = sd.readLine(file, buf, sizeof(buf), bytesRead);
      if(res != enSdRes::OK)
        break;
      m_count++;
    }
    sd.closeFile(file);
  }
}

void cnotes::getNote(int i, char* buf, size_t bufSize)
{
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  int count = 0;
  size_t bytesRead = 0;

  char fName[PAR_STR_LEN * 2];
  char line[PAR_STR_LEN];
  strncpy(fName, m_fName, sizeof (fName));
  strncat(fName, ".", 2);
  strncat(fName, devPars.lang.getActText(), 20);

  enSdRes res = sd.openFile(fName, file, enMode::READ);
  if(res == enSdRes::OK)
  {
    for(;;)
    {
      enSdRes res = sd.readLine(file, line, sizeof(line), bytesRead);
      if(res == enSdRes::OK)
      {
        if (count == i)
        {
          cUtils::replaceCharsInPlace(line, sizeof(line), '\n', 0);
          cUtils::replaceUTF8withInternalCoding(line, buf, bufSize);
          break;
        }
        else
          buf[0] = 0;
        count++;
      }
      else
        buf[0] = 0;
    }
  }
}

void cnotes::initListPar(cParList& list)
{
  for(int i = 0; i < m_count; i++)
  {
    char buf[PAR_STR_LEN];
    getNote(i, buf, sizeof (buf));
    list.addItem(buf);
  }
}

void cnotes::createFile(const char* fName, thText tHandleStart, thText tHandleEnd)
{
  cSdCard& sd = cSdCard::inst();
  tFILE file;
  enSdRes res = sd.openFile(fName, file, enMode::WRITE);
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

