#include "uSDFS.h"
#include "ff_utils.h"
/*
* for APL see http://elm-chan.org/fsw/ff/00index_e.html
*/
#define TEST_DRV 0
//
#if TEST_DRV == 0
  const char *Dev = "0:/";  // SPI
#elif TEST_DRV == 1
  const char *Dev = "1:/";  // SDHC
#elif TEST_DRV == 2
  const char *Dev = "2:/";  // USB
#endif

const char *FileNameTxt = "/Ascii/HELLO13.TXT";
const char *FileNameBin = "/Binary/test03.bin";

FRESULT rc;       /* Result code */
FATFS fatfs;      /* File system object */
FIL fil;          /* File object */
DIR dir;          /* Directory object */
FILINFO fno;      /* File information object */
UINT bw, br, wr;

TCHAR buff[128];
TCHAR *str;
#define BUFFSIZE (16*128) 
uint8_t buffer[BUFFSIZE];

void die(const char *text, FRESULT rc)
{ Serial.printf("%s: Failed with rc=%s.\r\n", text,FR_ERROR_STRING[rc]);  while(1) asm("wfi"); }

void setup() {
  // put your setup code here, to run once:
  while(!Serial);
  Serial.println("Test uSDFS");
  Serial.println(Dev);
  char2tchar(Dev, sizeof(Dev), buff);
  if((rc = f_mount (&fatfs, buff, 1))) die("Mount",rc);      /* Mount/Unmount a logical drive */

  //-----------------------------------------------------------
//  Serial.printf("\nChange drive\n");
//  if((rc = f_chdrive(buff))) die("chdrive",rc);

  //-----------------------------------------------------------
  Serial.printf("\nCreate a new subdirectories.\n");
  char2tchar("/Ascii", 7, buff);
  
  if((rc = f_mkdir(buff))) if(rc != FR_EXIST) die("mkDir Ascii",rc);
  char2tchar("/Binary", 8, buff);
  if((rc = f_mkdir(buff))) if(rc != FR_EXIST) die("mkDir Binary",rc);

  //-----------------------------------------------------------
  Serial.printf("\nCreate a new file %s.\n",FileNameTxt);
  char2tchar(FileNameTxt,sizeof(FileNameTxt), buff);
  if((rc = f_open(&fil, buff, FA_WRITE | FA_CREATE_ALWAYS))) die("Open",rc);

  Serial.println("Write some text lines. (Hello world!)");
  char2tchar("Hello world!\n",sizeof("Hello world!\n"), buff);
  bw = f_puts(buff, &fil);
  char2tchar("Second Line\n",sizeof("Second Line\n"), buff);
  bw = f_puts(buff, &fil);
  char2tchar("Third Line\n",sizeof("Third Line\n"), buff);
  bw = f_puts(buff, &fil);
  char2tchar("Fourth Line\n",sizeof("Fourth Line\n"), buff);
  bw = f_puts(buff, &fil);
  char2tchar("Habe keine Phantasie\n",sizeof("Habe keine Phantasie\n"), buff);
  bw = f_puts(buff, &fil);
  
  Serial.println("Close the file.");
  if((rc = f_close(&fil))) die("Close",rc);

  Serial.printf("\nOpen same file %s.\n",FileNameTxt);
  char2tchar(FileNameTxt,sizeof(FileNameTxt), buff);
  if((rc = f_open(&fil, buff, FA_READ))) die("Open",rc);

  Serial.println("Get the file content.");
  for (;;) 
  {
      if((str=f_gets(buff, sizeof(buff), &fil))) 
        Serial.printf("%s",str);
      else 
      { if(f_eof(&fil)) break; else die("f_gets",FR_DISK_ERR);} 
  }

  Serial.println("Close the file.");
  if((rc = f_close(&fil))) die("Close",rc);
  
  //-----------------------------------------------------------
  Serial.println("\nopen binary file");
      // open new file
      char2tchar(FileNameBin,sizeof(FileNameTxt), buff);
      if((rc = f_open(&fil, buff, FA_WRITE | FA_CREATE_ALWAYS))) die("Open",rc);
  Serial.println("write file");
       
       // fill buffer
       for(int ii=0;ii<BUFFSIZE;ii++) buffer[ii]='0';
       //write data to file 
       if((rc = f_write(&fil, buffer, BUFFSIZE, &wr))) die("Write1",rc);  
       
       // fill buffer
       for(int ii=0;ii<BUFFSIZE;ii++) buffer[ii]='1';
       //write data to file 
       if((rc = f_write(&fil, buffer, BUFFSIZE, &wr))) die("Write2",rc);  
       
      //close file
  Serial.println("close file");
      if((rc = f_close(&fil))) die("Close",rc);
  Serial.println("Binary test done");

  //-----------------------------------------------------------
  Serial.println("\nOpen root directory.");
  char2tchar(Dev, sizeof(Dev), buff);
  if((rc = f_opendir(&dir, buff))) die("Dir",rc);

  Serial.println("Directory listing...");
  for (;;) 
  {
      if((rc = f_readdir(&dir, &fno))) die("Listing",rc);   /* Read a directory item */
      if (!fno.fname[0]) break; /* Error or end of dir */
      if (fno.fattrib & AM_DIR)
           Serial.printf("   <dir>  %s\r\n", fno.fname);
      else
           Serial.printf("%8d  %s\r\n", (int)fno.fsize, fno.fname);
    delay(10);
  }

  Serial.println("\nTest completed.");

  if((rc = f_mount (&fatfs, buff, 1))) die("Mount",rc);      /* Mount/Unmount a logical drive */

  pinMode(13,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWriteFast(13,!digitalReadFast(13));
  delay(1000);
}
