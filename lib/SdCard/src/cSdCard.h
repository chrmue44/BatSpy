#ifndef CSDCARD_H
#define CSDCARD_H

#define CARDLIB_SDFAT
//#define CARDLIB_SD
//#define CARDLIB_USDFS


#if defined(CARDLIB_SD)
#define FILENAME_LEN        13   ///< max. length of a file name
#elif defined(CARDLIB_USDFS) || defined(CARDLIB_SDFAT)
#define FILENAME_LEN       256   ///< max. length of a file name
#endif
#define PATH_LEN           256   ///< max. length of a full path
#define MAX_DIRENTRIES      50
#define WRITE_BLOCK_SIZE  1024   ///< block size for file transfer from host to device

#if defined(CARDLIB_SD)
#include "SD.h"
#elif defined(CARDLIB_USDFS)
#include <wchar.h>
#include "ff.h"
#elif defined(CARDLIB_SDFAT)
#include "SdFat.h"
#define SD_FAT_TYPE 1
#define SD_CONFIG SdioConfig(FIFO_SDIO)

/*#define SPI_CLOCK SD_SCK_MHZ(50)
// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS
*/
#endif



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
};

enum enMode {
  READ,
  WRITE
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

#ifdef CARDLIB_SDFAT
typedef File32 tFILE;
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

   size_t available(tFILE& f)  { return f.available32();}
   bool eof(tFILE& f) { return f.available32() == 0;}
  
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
   enSdRes sendFileToTerminal(char* name);

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
  SdFat32 m_sd;
#endif
};

#endif //#ifndef CSDCARD_H
