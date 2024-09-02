#include "cSdCard.h"
#include <Arduino.h>
#include <SPI.h>
#include <TimeLib.h>
//#define DEBUG_LEVEL 1

#include "debug.h"
/*
#ifdef CARDLIB_SDFAT
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN

// Set PRE_ALLOCATE true to pre-allocate file clusters.
const bool PRE_ALLOCATE = true;

// Set SKIP_FIRST_LATENCY true if the first read/write to the SD can
// be avoid by writing a file header or reading the first record.
const bool SKIP_FIRST_LATENCY = true;

// Size of read/write.
const size_t BUF_SIZE = 512;

// File size in MB where MB = 1,000,000 bytes.
const uint32_t FILE_SIZE_MB = 5;

// Write pass count.
const uint8_t WRITE_COUNT = 2;

// Read pass count.
const uint8_t READ_COUNT = 2;
//==============================================================================
// End of configuration constants.
//------------------------------------------------------------------------------

#endif   //#ifdef CARDLIB_SDFAT
*/

#ifdef CARDLIB_USDFS
char * tchar2char( TCHAR * tcharString, int nn, char * charString)
{   int ii;
    for(ii = 0; ii<nn; ii++) 
    { charString[ii] = (char)tcharString[ii];
      if(!charString[ii]) 
        break;
    }
    return charString;
}


TCHAR * char2tchar( const char * charString, int nn, TCHAR * tcharString) {
  for(int i = 0; i < nn; i++) {
    tcharString[i] = (TCHAR)charString[i];
    if(!tcharString[i]) 
      break;
  }
  return tcharString;
}
#endif //#ifdef CARDLIB_USDFS


cSdCard sdc;

cSdCard& cSdCard::inst() {
  return sdc;
}


cSdCard::cSdCard() {
  memset(m_actDir,0,sizeof(m_actDir));
#ifdef CARDLIB_USDFS
  memset(&m_fatfs,0,sizeof(m_fatfs));
#endif
}


enSdRes cSdCard::mount() 
{
  enSdRes retVal = OK;
#if defined(CARDLIB_SDFAT)
  if (!m_sd.begin(SD_CONFIG))
  {
    retVal = MOUNT_FAILED;
    m_ok = false;
  }
  else
    m_ok = true;
  strcpy(m_actDir, "/");

#elif defined(CARDLIB_SD)
  if (!SD.begin(BUILTIN_SDCARD))
    retVal = MOUNT_FAILED;
  strcpy(m_actDir, "/");

#elif  defined(CARDLIB_USDFS)
  TCHAR tdir[FILENAME_LEN];
  strcpy(m_actDir, "0:/");
  char2tchar(m_actDir, sizeof(m_actDir), tdir);
  Serial.println(m_actDir);
  FRESULT rc = f_mount(&m_fatfs, tdir, 1);      /* Mount/Unmount a logical drive */  
  DPRINTF1("return value: %i\n", rc);
  retVal = rc == FR_OK ? OK : MOUNT_FAILED;
#endif  
  Serial.println(m_actDir);
  return retVal;
}

enSdRes cSdCard::unmount()
{
  enSdRes retVal = OK;
  if(m_ok)
  {
  #if defined(CARDLIB_SD)
    ;
   #elif defined(CARDLIB_SDFAT)
    int count = 0;
    while(m_sd.isBusy() && (count < 100))
    {
      delay(2);
      count++;
    }
    m_sd.begin(1);
    m_ok = false;
#elif defined(CARDLID_USDFS)
  TCHAR tdir[FILENAME_LEN];
  char2tchar(m_actDir, sizeof(m_actDir), tdir);
  FRESULT rc = f_mount (NULL, tdir, 0);      /* Mount/Unmount a logical drive */
  if(rc != FR_OK)
    retVal = UNMOUNT_ERR;
#endif
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  return retVal;
}

#if defined(CARDLIB_SDFAT)
enSdRes cSdCard::dir(tDirInfo& directory, bool terminal, const char* ext, size_t startIndex) 
{
  enSdRes retVal = OK;
  if(m_ok)
  {
  tFILE dir;
  tFILE file;
 
  if (!dir.open(m_actDir))
    return OPEN_DIR;

  if(terminal && (startIndex == 0))
    Serial.printf("dir: %s\n", m_actDir);
  m_dirInfo.clear();
  stDirEntry d;
  bool exitFlag = false;
  for(size_t i = 0; i < startIndex; i++) {
    bool ok = file.openNext(&dir, O_RDONLY);
    if (!ok) {
      exitFlag = true;
      break;
    }
    file.close();
  }

  if(exitFlag)
    retVal =  END_OF_DIR;
  else
  {
    for (int i = 0; i < MAX_DIRENTRIES; i++) {
      bool ok = file.openNext(&dir, O_RDONLY);
      if (! ok) {
        if(terminal)
          Serial.printf("dir end\n");
        break; // Error or end of dir 
      }
 
      if(ext != NULL) {
        char* p = strrchr (d.name, '.');
        if(p == NULL)
          continue;
        p++;
        if(strcmp(p, ext) != 0)
          continue;
      }

      file.getName(d.name, FILENAME_LEN);
      if (file.isDir()) {
        d.isDir = true; 
        d.fSize = 0;
        Serial.printf("[ %s ]\n", d.name);
      }
      else {
        d.isDir = false; 
        d.fSize = file.fileSize();
        Serial.printf("%s : %u\n", d.name, d.fSize);
      }
      m_dirInfo.push_back(d);
      file.close();
    }
    d.isDir = true; 
    d.fSize = 0;
    strcpy(d.name,"..");
    m_dirInfo.push_back(d);      
    dir.close();
  } 
  directory = m_dirInfo;
  }
  else
    retVal = enSdRes::MOUNT_FAILED; 
  return retVal;
}

#elif defined(CARDLIB_SD)
enSdRes cSdCard::dir(tDirInfo& directory, bool terminal, const char* ext, size_t startIndex) 
{
  enSdRes retVal = OK;
  File dir = SD.open(m_actDir);
  if (!dir)
    return OPEN_DIR;

  if(terminal && (startIndex == 0))
    Serial.printf("dir: %s\n", m_actDir);
  m_dirInfo.clear();
  stDirEntry d;
  bool exitFlag = false;
  for(size_t i = 0; i < startIndex; i++) {
    File entry =  dir.openNextFile();
    if (! entry) {
      exitFlag = true;
      break;
    }
  }

  if(exitFlag)
    retVal =  END_OF_DIR;
  else
  {
    for (int i = 0; i < MAX_DIRENTRIES; i++) {
      File entry =  dir.openNextFile();
      if (! entry) {
        if(terminal)
          Serial.printf("dir end\n");
        break; // Error or end of dir 
      }
 
      if(ext != NULL) {
        char* p = strrchr (d.name, '.');
        if(p == NULL)
          continue;
        p++;
        if(strcmp(p, ext) != 0)
          continue;
      }

      strncpy(d.name, entry.name(), FILENAME_LEN);
      if (entry.isDirectory()) {
        d.isDir = true; 
        d.fSize = 0;
        Serial.printf("[ %s ]\n", d.name);
      }
      else {
        d.isDir = false; 
        d.fSize = entry.size();
        Serial.printf("%s : %u\n", d.name, d.fSize);
      }
      m_dirInfo.push_back(d);
//    delay(3);
//    delay(30);
    }
    d.isDir = true; 
    d.fSize = 0;
    strcpy(d.name,"..");
    m_dirInfo.push_back(d);      
    dir.close();
  } 
  directory = &m_dirInfo; 
  return retVal;
}

#elif defined(CARDLIB_USDFS)
enSdRes cSdCard::dir(tDirInfo*& directory, bool terminal, const char* ext, size_t startIndex) {
  enSdRes retVal = OK;
  DIR dir;                       ///< Directory object
  TCHAR tdir[FILENAME_LEN];
  FILINFO fno;                   ///< File information object

  char2tchar(m_actDir, sizeof(m_actDir), tdir);
  FRESULT rc = f_opendir(&dir, tdir);
 
  if (rc)
    return OPEN_DIR;

  if(terminal && (startIndex == 0))
    Serial.printf("dir: %s\n", m_actDir);
  m_dirInfo.clear();
  stDirEntry d;
  bool exitFlag = false;
  for(size_t i = 0; i < startIndex; i++) {    
    rc = f_readdir(&dir, &fno);
    if (rc || !fno.fname[0]) {
      exitFlag = true;
      break;
    }
  }
  if(exitFlag)
    retVal =  END_OF_DIR;
  else
  {
    for (int i = 0; i < MAX_DIRENTRIES; i++) {
      DPRINTLN1("dir0");
      rc = f_readdir(&dir, &fno);   // Read a directory item 
      DPRINTLN1("dir1");
      if (rc || !fno.fname[0]) {
        if(terminal)
          Serial.printf("dir end\n");
        break; // Error or end of dir 
      }
      DPRINTLN1("dir2");
      tchar2char(fno.fname,FILENAME_LEN, d.name);
      DPRINTLN1("dir3");
      if(ext != NULL) {
        char* p = strrchr (d.name, '.');
        if(p == NULL)
          continue;
        p++;
        if(strcmp(p, ext) != 0)
          continue;
      }

      DPRINTLN1("dir3");
      if (fno.fattrib & AM_DIR) {
        d.isDir = true; 
        d.fSize = 0;
        Serial.printf("[ %s ]\n", d.name);
      }
      else {
        d.isDir = false; 
        d.fSize = fno.fsize;
        Serial.printf("%s : %u\n", d.name, d.fSize);
      }
      m_dirInfo.push_back(d);      
      DPRINTLN1("dir4");
//    delay(3);
//    delay(30);
      DPRINTLN1("dir4.1");
    }
  }
  d.isDir = true; 
  d.fSize = 0;
  strcpy(d.name,"..");
  DPRINTLN1("dir5");
  m_dirInfo.push_back(d);      
  rc = f_closedir(&dir);
  DPRINTLN1("dir6");
//  delay(10);
  directory = &m_dirInfo; 
  DPRINTLN1("dir7");
  return retVal;
}
#endif


enSdRes cSdCard::mkDir(const char* name)
{
  if(m_ok)
  {
  DPRINTF1("mkdir( %s )\n", name);
#if defined(CARDLIB_SD)  
  char buf[PATH_LEN];
  transformPathName(buf, PATH_LEN, name);
  bool rc = SD.mkdir(buf);
  return rc ? OK : MKDIR_ERR;
  
#elif defined(CARDLIB_USDFS)  
  TCHAR buf[FILENAME_LEN];
  transformPathName(buf, PATH_LEN, name);
  FRESULT rc =  f_mkdir (buf);  
  return rc == FR_OK ? OK : MKDIR_ERR;

#elif defined(CARDLIB_SDFAT)  
  char buf[PATH_LEN];
  transformPathName(buf, PATH_LEN, name);
  DPRINTF1("dir to create: %s\n", buf); 
  bool rc = m_sd.mkdir(buf);
  return rc ? OK : MKDIR_ERR;
  }
  else
    return enSdRes::MOUNT_FAILED;
#endif
}


enSdRes cSdCard::del(const char* name)
{
  enSdRes retVal = OK;
  if(m_ok)
  {
  #if defined(CARDLIB_SD)  
  char buf[PATH_LEN];
  transformPathName(buf, PATH_LEN, name);    
  bool rc =  SD.remove(buf);
  retVal = rc ? OK : DEL_ERR;
#elif defined(CARDLIB_USDFS)    
  TCHAR buf[FILENAME_LEN];
  transformPathName(buf, FILENAME_LEN, name);    
  FRESULT rc = f_unlink(buf);
  retVal = rc == FR_OK ? OK : DEL_ERR;
#elif defined(CARDLIB_SDFAT)
  char buf[PATH_LEN];
  transformPathName(buf, PATH_LEN, name);    
  bool rc =  m_sd.remove(buf);
  retVal = rc ? OK : DEL_ERR;
#endif
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  return retVal;
}

enSdRes cSdCard::rename (const char* oldName, const char* newName)
{
  enSdRes retVal = enSdRes::OK;
  if(m_ok)
  {
#if defined(CARDLIB_SDFAT)
  bool ok = m_sd.rename(oldName, newName);
  if(!ok)
    retVal = enSdRes::RENAME_ERR;
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  return retVal;
#endif
}

enSdRes cSdCard::deldir(const char* name) 
{
  if(m_ok)
  {
#if defined(CARDLIB_SD)  
  char buf[PATH_LEN];
  transformPathName(buf, PATH_LEN, name);    
  bool rc = SD.rmdir(buf);
  return rc ? OK : DEL_ERR;
#elif defined(CARDLIB_USDFS)    
  return del(name);
#elif defined(CARDLIB_SDFAT)
  char buf[PATH_LEN];
  transformPathName(buf, PATH_LEN, name);    
  bool rc = m_sd.rmdir(name);
  return rc ? OK : DEL_ERR;
#endif
  }
  else
    return enSdRes::MOUNT_FAILED;
}

#if defined(CARDLIB_USDFS)  
enSdRes cSdCard::rename(const char* oldName, const char* newName) {
  TCHAR buf_old[PATH_LEN];
  TCHAR buf_new[PATH_LEN];
  transformPathName(buf_old, PATH_LEN, oldName);    
  transformPathName(buf_new, PATH_LEN, newName);
  FRESULT rc = f_rename(buf_old, buf_new);   
  return rc == FR_OK ? OK : RENAME_ERR;  
}
#endif


enSdRes cSdCard::chdir(const char* name)
{
  DPRINTF1("chdir( %s )\n", name);
  enSdRes retVal = OK;
  if(m_ok)
  {
  char dir[PATH_LEN];
  
  strcpy(dir, m_actDir);
  
  // one directory level up
  if(strcmp(name, "..") == 0) {
    char* last = strrchr(dir, '/');
    if((last == &dir[0])) {
      dir[1] = 0;
    }
    else
      *(last) = 0;
  }
  // absolute path
#if defined(CARDLIB_SD) || defined(CARDLIB_SDFAT)
  else if(name[0] == '/') {
    strncpy(dir, name, sizeof(dir) - 1);
#elif defined(CARDLIB_USDFS)
  else if((name[0] == '0') && (name[1] == ':')) {
#endif
  }  
  // relative path
  else {
    int dirlen = strlen(dir);
    if (dir[dirlen - 1] != '/')
      strcat(dir,"/");
    strcat(dir,name);
  }
    
#if defined(CARDLIB_SD)
  File dirFile = SD.open(m_actDir);
  if (!dirFile)
    retVal = OPEN_DIR;

#elif defined(CARDLIB_USDFS)
  TCHAR tdir[FILENAME_LEN];  
  DIR dirO;                       ///< Directory object
  FRESULT rc = f_opendir(&dirO, char2tchar(dir, sizeof(dir), tdir));
  retVal = rc == FR_OK ? OK : OPEN_DIR;

#elif defined(CARDLIB_SDFAT)
  tFILE dirI;
  if(!dirI.open(dir))
    retVal = OPEN_DIR;
  dirI.close();
#endif
  if(retVal == OK)
    strcpy(m_actDir, dir);
  DPRINTF1("curr dir: %s\n", m_actDir);
  DPRINTF1("returns: %i\n", retVal);
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  return retVal;
}

/* f_getcwd seems to be not implemented...
char* cSdCard::getActPath() {
  TCHAR buf[PATH_LEN];
  enSdRes rc = f_getcwd(buf, sizeof(buf));
  tchar2char(buf, sizeof(buf), m_actDir);
  return m_actDir;
}
*/
enSdRes cSdCard::openFile(const char* name, tFILE& file, enMode mode) 
{
  enSdRes retVal = OK;
  if(m_ok)
  {
#ifdef DEBUG_LEVEL
  if (mode == READ)
    DPRINTF1("openFile( %s, tFILE, READ)\n", name);
  if (mode == WRITE)
    DPRINTF1("openFile( %s, tFILE, WRITE)\n", name);
#endif
#if defined(CARDLIB_SD)
  char buf[PATH_LEN];
  transformPathName(buf, PATH_LEN, name);
  if (mode == READ)
    file = SD.open(buf, FILE_READ);
  else
    file = SD.open(buf, FILE_WRITE);
  if(!file)
     retVal = OPEN_FILE_ERR;
  
#elif defined (CARDLIB_USDFS)
  TCHAR buf[FILENAME_LEN];
  transformPathName(buf, FILENAME_LEN, name);
  FRESULT rc = f_open (&file, buf, mode);        
  retVal = rc == FR_OK ? OK : OPEN_FILE_ERR;
  
#elif defined (CARDLIB_SDFAT)
  char buf[PATH_LEN];
  transformPathName(buf, PATH_LEN, name);
  DPRINTF1("open file: %s\n", buf);
  bool ok = false;
  switch(mode)
  {
    case enMode::READ:
      ok = file.open(buf, O_RDONLY);
      break;
    case enMode::WRITE:
      del(name);
      ok = file.open(buf, FILE_WRITE);
      break;
    case enMode::APPEND:
      ok = file.open(buf, (O_WRONLY | O_APPEND));
      break;
  }
  retVal = ok ? enSdRes::OK : enSdRes::OPEN_FILE_ERR;
#endif
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  DPRINTF1("returns %i\n", retVal);  
  return retVal;
}

enSdRes cSdCard::readFile(tFILE& file, void* buf, size_t& bytesRead, size_t bytesToRead)
{
  enSdRes retVal = OK;
  if(m_ok)
  {
  DPRINTF1("readFile(tFILE, buf, bytesRead, %i)\n", bytesToRead);
#if defined(CARDLIB_SD)
  int rc = file.read(buf, bytesToRead);
  if(rc < 0)
    retVal = READ_ERR;
  else
    bytesRead = (size_t)rc;

#elif defined(CARDLIB_USDFS)
  FRESULT rc = f_read (&file, buf, bytesToRead, &bytesRead); 
  retVal = rc == FR_OK ? OK : READ_ERR;

#elif defined(CARDLIB_SDFAT)
  int rc = file.read(buf, bytesToRead);
  if(rc < 0) {
    retVal = READ_ERR;
    DPRINTF1("internal rc %i\n", rc);  
  }
  else
    bytesRead = (size_t)rc;
#endif
  DPRINTF1("returns %i\n", retVal);  
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  return retVal;
}


enSdRes cSdCard::readLine(tFILE& file, void* buf, size_t bufSize, size_t& bytesRead)
{
  enSdRes res;
  if(m_ok)
  {
    char* pBuf = (char*)buf;
    bytesRead = 0;
    size_t count;
    while (pBuf < ((char*)buf + bufSize - 1))
    {
      res = readFile(file, pBuf, count, 1);
      if((count == 0 ) || (res != enSdRes::OK))
        break;
      bytesRead += count;
      if(*pBuf == '\n')
        break;
      pBuf++;
    }
    if(*pBuf == '\n')
    {
      pBuf++;
      *pBuf = 0;
    }
    else
      res = enSdRes::LINE_ERR;
  }
  else
    res = enSdRes::MOUNT_FAILED;
  return res;
}


enSdRes cSdCard::writeFile(tFILE& file, const void* buf, size_t& bytesWritten, size_t bytesToWrite)
{
  enSdRes retVal = OK;
  if(m_ok)
  {  
#if defined(CARDLIB_SD)
  int rc = file.write((const uint8_t*)buf, bytesToWrite);
  if(rc < 0)
    retVal = WRITE_ERR;
  else
    bytesWritten = (size_t)rc;
  
#elif defined(CARDLIB_USDFS)
  FRESULT rc = f_write (&file, buf, bytesToWrite, &bytesWritten);
  retVal = rc == FR_OK ? OK : WRITE_ERR;

#elif defined(CARDLIB_SDFAT)
  int rc = file.write((const uint8_t*)buf, bytesToWrite);
  DPRINTF1("wr file: %lu\n", bytesToWrite);
  if(rc < 0)
    retVal = WRITE_ERR;
  else
    bytesWritten = (size_t)rc;
#endif
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  return retVal;  
}

bool cSdCard::fileExists(char* fName)
{
  tFILE file;
  bool retVal = false;
  if(m_ok)
  {
    enSdRes res = openFile(fName, file, enMode::READ);
    if(res == enSdRes::OK)
    {
      retVal = true;
      closeFile(file);
    }
  }
  return retVal;
  //return m_sd.exists(fName);
}


enSdRes cSdCard::closeFile(tFILE& file)
{
  enSdRes retVal = OK;  
  if(m_ok)
  {
  DPRINTLN1("closeFile(tFILE)\n");

  #if defined(CARDLIB_SD)
  file.close();

  #elif defined(CARDLIB_USDFS)
  FRESULT rc = f_close (&file); 
  retVal = rc == FR_OK ? OK : CLOSE_ERR;

  #elif defined(CARDLIB_SDFAT)
  
  file.timestamp(T_WRITE,year(), month(), day(), hour(), minute(), second());
  bool ok = file.close();
  retVal = ok ? OK : CLOSE_ERR;
  #endif
  DPRINTF1("returned %i\n", (int)retVal);
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  return retVal; 
}

#if defined(CARDLIB_SD) || defined(CARDLIB_SDFAT)
void cSdCard::transformPathName(char* buf, size_t bufsize, const char* name) {
  char path[PATH_LEN];
  if(name[0] == '/') {
    strncpy(buf, name, bufsize);
  }
  else {
    strcpy(path, m_actDir);
    int len = strlen(path);
    if (path[len - 1] != '/')
      strcat(path,"/");
    strcat(path, name);
    strncpy(buf, path, bufsize);
  }
}

#elif defined(CARDLIB_USDFS)
void cSdCard::transformPathName(TCHAR* buf, UINT bufsize, const char* name) {
  char path[FILENAME_LEN];
  if((name[0] == '0') && (name[1] == ':')) {
    char2tchar(name, bufsize, buf);
  }
  else {
    strcpy(path, m_actDir);
    int len = strlen(path);
    if (path[len - 1] != '/')
      strcat(path,"/");
    strcat(path, name);
    char2tchar(path, bufsize, buf);
  }
}
#endif


#define BUFLEN  64

enSdRes cSdCard::sendFileToTerminal(char* name, int delayTime) 
{
  enSdRes rc = enSdRes::OK;
  if(m_ok)
  {
    tFILE file;
    char buf[BUFLEN + 2];
    size_t bytesRead = 0;
    rc = openFile(name, file, READ);
    if(rc == OK) 
    {  
      size_t size = fileSize(file);
      Serial.printf("%09u:",size);
      int cnt = 0;
      while(! eof(file))
     {
        rc = readFile(file, &buf, bytesRead, BUFLEN);
        Serial.write(&buf[0],bytesRead);
        cnt++;
        if(cnt >= 32)
        {
          cnt = 0;
          delay(delayTime);
        }
      }
    }
  }
  else
    rc = enSdRes::MOUNT_FAILED;
  return rc;
}


enSdRes cSdCard::readFileFromTerminal(char* name) 
{
  tFILE file;
  char buf[80];
  size_t bytesLeft;
  size_t bytesWritten = 0;
  enSdRes rc = OK;
  m_fileTransferActive = true;
  uint32_t startTime = millis();
  Serial.print("0"); //send reply to start file transfer

  while(Serial.available() < 10) {
    if((millis() - startTime) > 5000) {
      Serial.print("timeout receiving file");
      m_fileTransferActive = false;
      return INT_ERR;
    }
  }
  for(int i = 0; i < 10; i++)
    buf[i] = Serial.read();

  if(buf[9] == ':') {
    Serial.print("0");
    rc = openFile(name, file, WRITE);
    buf[9] = 0;
    bytesLeft = atoi(buf);
 
    while(bytesLeft > 0) {
      startTime = millis();
      size_t bytesToRead = WRITE_BLOCK_SIZE;
      if(bytesLeft < bytesToRead)
      bytesToRead = bytesLeft;
      size_t written = 0;
      while(bytesToRead > 0) {
        while(Serial.available() < 1) {
          if((millis() - startTime) > 5000) {
            Serial.print("2 timeout receiving file");
            m_fileTransferActive = false;
            closeFile(file);
            return INT_ERR;
          }
        }          
        buf[0] = Serial.read();
        rc = writeFile(file, &buf, bytesWritten, 1);
        written += bytesWritten;
        bytesToRead--;
        bytesLeft--;
      }
      Serial.printf("0 bytes written: %i", written);
    }
    rc = closeFile(file);
  }
  else {
    rc = INT_ERR;
    Serial.print("1");
  }
  m_fileTransferActive = false;
  return rc;
}


enSdRes cSdCard::getFreeMem(size_t& freeSpaceKb, size_t& totSpaceKb)
{
  enSdRes retVal = OK;
  if(m_ok)
  {
#if defined(CARDLIB_USDFS) 
  FATFS *fs;
  DWORD fre_clust, fre_sect, tot_sect;

  char cbuf[10];
  TCHAR tbuf[10];
  strcpy(cbuf,"0:");
  transformPathName(tbuf, 10, cbuf);

  // Get volume information and free clusters of drive 1 
  FRESULT res = f_getfree(tbuf, &fre_clust, &fs);
  if (res == 0) {
    // Get total sectors and free sectors 
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;
    freeSpaceKb = fre_sect / 2;
    totSpaceKb = tot_sect / 2;
    // Print the free space (assuming 512 bytes/sector) 
//    printf("%10lu KiB total drive space.\n%10lu KiB available.\n",
//           tot_sect / 2, fre_sect / 2);
  }
#elif defined(CARDLIB_SDFAT)
  size_t kbPerCluster = m_sd.bytesPerCluster() / 1024;
  freeSpaceKb = m_sd.freeClusterCount() * kbPerCluster;
  totSpaceKb = m_sd.clusterCount() * kbPerCluster;
#endif
  }
  else
  {
    retVal = enSdRes::MOUNT_FAILED;
    freeSpaceKb = 0;
    totSpaceKb = 0;
  }
  return retVal;
}


enSdRes cSdCard::format()
{
  enSdRes retVal = enSdRes::OK; 
  if(m_ok)
  {
#if SD_FAT_TYPE == 2 
//  SdExFat sd;
  if (!m_sd.cardBegin(SD_CONFIG)) 
  {
    DPRINTLN1("cardBegin failed\n");
    retVal = enSdRes::FORMAT_ERR;
  }
  delay(100);
  if (!m_sd.volumeBegin()) 
  {
    DPRINTLN1("volumeBegin failed\n");
    retVal = enSdRes::FORMAT_ERR;
  }
  if(!m_sd.format(&Serial))
  {
    Serial.printf("format failed\n");
  }
  if (!m_sd.volumeBegin()) 
  {
    DPRINTLN1("volumeBegin failed\n");
  }
  DPRINTF1("Bytes per cluster: %l\n",m_sd.bytesPerCluster());
  chdir("/");
  DPRINTLN1("Done");
#endif
  }
  else
    retVal = enSdRes::MOUNT_FAILED;
  return retVal;
}
