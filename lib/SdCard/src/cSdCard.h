#ifndef CSDCARD_H
#define CSDCARD_H

#define CARDLIB_SDFAT

#include <Arduino.h>

#define FILENAME_LEN       256   ///< max. length of a file name
#define PATH_LEN           256   ///< max. length of a full path
#define MAX_DIRENTRIES      50
#define WRITE_BLOCK_SIZE  1024   ///< block size for file transfer from host to device


#include <SdFat.h>
#define SD_FAT_TYPE 2
#ifdef ARDUINO_TEENSY41
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#endif
#ifdef ARDUINO_TEENSY40
const uint8_t SD_CS_PIN = 23;
#define SPI_CLOCK SD_SCK_MHZ(40)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#endif

#if SD_FAT_TYPE == 0
typedef SdFat sd_t;
typedef File tFILE;
#elif SD_FAT_TYPE == 1
typedef SdFat32 sd_t;
typedef File32 tFILE;
#elif SD_FAT_TYPE == 2
typedef SdExFat sd_t;
typedef ExFile tFILE;
#elif SD_FAT_TYPE == 3
typedef SdFs sd_t;
typedef FsFile tFILE;
#else  // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE


#include <my_vector.h>


enum enSdRes
{
  OK = 0,
  MOUNT_FAILED = 1,
  OPEN_DIR = 2,
  END_OF_DIR = 3,
  READ_ERR = 4,
  WRITE_ERR = 5,
  LINE_ERR = 6,
  CLOSE_ERR = 7,
  MKDIR_ERR = 8,
  DEL_ERR = 9,
  OPEN_FILE_ERR = 10,
  INT_ERR = 11,
  UNMOUNT_ERR = 12,
  RENAME_ERR = 13,
  SEEK_ERR = 14,
  FORMAT_ERR = 15,
};

enum enMode {
  READ,
  WRITE,
  APPEND,
};

struct stDirEntry {
  char name[FILENAME_LEN];
  size_t fSize;
  bool isDir;
};

typedef my_vector<stDirEntry, MAX_DIRENTRIES+1> tDirInfo;

#ifdef CARDLIB_SD
typedef File tFILE;
#endif

/**
 * wrapper class for convenient use of different SD card libs
 **/
class cSdCard {
 public:
   static cSdCard& inst();
   cSdCard();

   bool isFileTransferActive() { return m_fileTransferActive; }

   enSdRes mount();
   
   enSdRes unmount();

  /**
   * get directory entries
   * reads directory entries to output variable directory as long the capacity of the output variable allows
   * To read big directories that exceed the capacity use parameter 'startIndex' to read the directory in 
   * several pieces
   *
   * directory: vector to store diectory entries
   * terminal: true: generate terminal output
   * ext: file extension to filter directory entries (NULL = no filter)
   * startIndex: index from which the directory entries are filled
   */
   enSdRes dir(tDirInfo& directory , bool terminal = false, const char* ext = NULL, size_t startIndex = 0);
   
   enSdRes mkDir(const char* name);

   enSdRes del(const char* name);

   enSdRes deldir(const char* name);

   /**
    * change directory:
    * 1. "0:/...": absolute path
    * 2. "..": one level up ("../.." not possible)
    * 3. "name1/name2": relative path from actual dir
    */
   enSdRes chdir(const char* name);
   
   char* getActPath();
   
   char* getActDir() { return m_actDir; }

  /**
    * open a file
    *
    **/
   enSdRes openFile(const char* name, tFILE& file, enMode mode);
   
   enSdRes readFile(tFILE& file, void* buf, size_t& bytesRead, size_t bytesToRead);

   enSdRes readLine(tFILE& file, void* buf, size_t bufSize, size_t& bytesRead);

   enSdRes writeFile(tFILE& file, const void* buf, size_t& bytesWritten, size_t bytesToWrite);

   enSdRes closeFile(tFILE& file);

   size_t fileSize(tFILE& file) { return file.fileSize(); }

   size_t available(tFILE& f)
   {
  #if SD_FAT_TYPE == 1
      return f.available32();
  #endif
  #if SD_FAT_TYPE == 2
     return f.available();
  #endif
    }

   bool eof(tFILE& f)
   {
     return available(f) == 0;
   }
  
   bool fileExists(char* fName)
   {
     bool retVal = false;
     tFILE f;
     enSdRes res = openFile(fName, f, enMode::READ);
     if(res == enSdRes::OK)
     {
         closeFile(f);
         retVal = true;
     }
     return retVal;
   }
   /* 
    * set file position
    */
   enSdRes setFilePos(tFILE& f, size_t pos) { return f.seekSet(pos) ? OK : SEEK_ERR; }

   size_t getFilePos(tFILE& f) { return f.curPosition();}

   /*
    * send file with given name to serial interface
    * format length 9 digit decimal as ASCII followed by ':'
    * then file content in binary format
    */
   enSdRes sendFileToTerminal(char* name, int delayTime);

   /*
    * read file with given name from serial interface
    * format: length 9 digit decimal as ASCII followed by ':'
    * then file content in binary format
    */
   enSdRes readFileFromTerminal(char* name);

   /*
    * rename a file or directory
    */
   enSdRes rename (const char* oldName, const char* newName);

   /*
    * get free memory in kByte
    */
   enSdRes getFreeMem(size_t& freeSpaceKb, size_t& totSpacekB);

   /*
    * format SD card
    */  
   enSdRes format();
 protected:
   /**
    * transform path name
    * if path starts with "0:", result is simply <name> transformed to TCHAR (absolute path)
    * if path is relative, result is <actDir>/<name> transformed to TCHAR
    */
#if defined(CARDLIB_SD) || defined(CARDLIB_SDFAT)
  void transformPathName(char* buf, size_t bufsize, const char* name);
#elif defined(CARDLIB_USDFS)
  void transformPathName(TCHAR* buf, UINT bufsize, const char* name);
#endif

 private:  
  char m_actDir[PATH_LEN];           ///< actual directory (no trailing '/' if in sub dir)
  tDirInfo m_dirInfo;                ///< content of actual directory
  bool m_fileTransferActive = false; ///< indicates that a file transfer is running and serial line should not be used by others
#if defined(CARDLIB_USDFS)
  FATFS m_fatfs;                     ///< File system object
#elif defined(CARDLIB_SDFAT)
  sd_t m_sd;
#endif
};

#endif //#ifndef CSDCARD_H
